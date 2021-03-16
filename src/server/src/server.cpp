#include <server/server.h>

void Connection::write(const Event &event) {
    Writer writer;
    event.write(writer);

    Container container;
    container.type = event.type();
    container.size = writer.data.size();

    const uint8_t *containerPtr = reinterpret_cast<const uint8_t *>(&container);

    std::vector<uint8_t> data;
    data.reserve(sizeof(container) + writer.data.size());
    data.insert(data.end(), containerPtr, containerPtr + sizeof(container));
    data.insert(data.end(), writer.data.begin(), writer.data.end());

    socket.async_send(asio::buffer(data), [](asio::error_code, std::size_t) { });
}

void Connection::announce(const Event &event) {
    std::lock_guard guard(server.mutex);

    for (const auto &c : server.connections) {
        if (c->playerId == playerId)
            continue;

        c->write(event);
    }
}

void Connection::handle(const Message &message) {
    struct {
        Connection &connection;

        void operator()(const messages::Hello &h) { throw std::exception(); }

        void operator()(const messages::Move &m) {
            assert(m.playerId == connection.playerId);

            connection.announce(m);
        }

        void operator()(const messages::Replace &r) {
            connection.server.blocks[r.x + connection.server.options.worldWidth * r.y]
                = r.block < 0 ? nullptr : connection.server.blockList[r.block];

            connection.announce(r);
        }
    } visitor { *this };

    std::visit(visitor, message);
}

void Connection::check() {
    Message message;

    Container c;
    {
        std::lock_guard guard(mutex);

        if (bytes < sizeof(Container))
            return;

        c = reinterpret_cast<Container &>(*buffer.data());

        if (c.size + sizeof(Container) < bytes)
            return;

        message = parse(c, &buffer[sizeof(Container)]);
    }

    handle(message);

    {
        std::lock_guard guard(mutex);

        size_t parsed = sizeof(Container) + c.size;

        buffer.erase(buffer.begin(), buffer.begin() + parsed);
        bytes -= parsed;
    }
}

void Connection::listen() {
    constexpr size_t constant = 1024;

    {
        std::lock_guard guard(mutex);

        buffer.resize(buffer.size() + std::max(buffer.size() - bytes, constant));
    }

    auto respond = [this](asio::error_code e, std::size_t n) {
        bytes += n;

        check();

        if (e != asio::error::eof)
            listen();
    };

    socket.async_read_some(asio::buffer(&buffer[bytes], buffer.size() - bytes), respond);
}

Connection::Connection(size_t playerId, tcp::socket socket, Server &server, messages::Hello hello)
    : playerId(playerId), server(server), socket(std::move(socket)), hello(std::move(hello)) { }

void Server::accept(tcp::acceptor &acceptor) {
    temp = std::make_unique<tcp::socket>(context);

    acceptor.async_accept(*temp, [this, &acceptor](asio::error_code error) {
        {
            std::lock_guard guard(mutex);

            size_t playerId = connections.size();
            connections.push_back(std::make_unique<Connection>(playerId, std::move(*temp), *this, messages::Hello {
                connections.size(), 0.5f, 0.5f,
                options.worldWidth, options.worldHeight,
                blocks::encode(blocks, blockIndices)
            }));

            Connection &c = *connections[playerId];

            c.write(c.hello);
            c.listen();

            for (const auto &e : connections) {
                if (e->playerId == playerId)
                    continue;

                e->write(messages::Move{
                    playerId, c.hello.playerX, c.hello.playerY
                });
            }
        }

        accept(acceptor);
    });
}

void Server::run() {
    tcp::acceptor acceptor(context, tcp::endpoint(tcp::v4(), 23788));

    accept(acceptor);

    context.run();
}

Server::Server(const Options &options) : options(options) {
    blockList = blocks::getBlocks();
    blockIndices = blocks::getIndices();

    blocks = blocks::generate(options.worldWidth, options.worldHeight);
}
