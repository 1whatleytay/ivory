#include <server/server.h>

#include <fmt/printf.h>

#include <iostream>

void Connection::markDisconnected() {
    if (!disconnected) {
        disconnected = true;
        fmt::print("Connection [{:0>4}] disconnected.\n", playerId);
        announce(messages::Disconnect{ playerId });
    }
}

void Connection::write(const Event &event) {
    Writer writer;
    event.write(writer);

    size_t fullSize = sizeof(Container) + writer.data.size();
    auto data = std::make_unique<uint8_t[]>(fullSize);
    *reinterpret_cast<Container *>(data.get()) = { event.type(), writer.data.size() };
    std::memcpy(&data[sizeof(Container)], writer.data.data(), writer.data.size());
    uint8_t* ptr = data.get();

    asio::async_write(socket, asio::buffer(ptr, fullSize), [this, fullSize, data { std::move(data) }](
        asio::error_code ec, std::size_t n
    ) {
        if (ec == asio::error::eof || n != fullSize)
            markDisconnected();
    });
}

void Connection::announce(const Event &event) {
    std::lock_guard guard(server.mutex);

    for (const auto &c : server.connections) {
        if (c->playerId == playerId)
            continue;

        if (c->disconnected)
            continue;

        c->write(event);
    }
}

void Connection::handle(const Message &message) {
    struct {
        Connection &connection;

        void operator()(const messages::Hello &h) { throw std::exception(); }
        void operator()(const messages::Disconnect &d) { throw std::exception(); }

        void operator()(const messages::PickUp &p) {
            assert(p.playerId == connection.playerId);

            connection.announce(p);
        }

        void operator()(const messages::Capture &c) {
            assert(c.color != connection.hello.color);

            connection.announce(c);
        }

        void operator()(const messages::Move &m) {
            assert(m.playerId == connection.playerId);

            connection.announce(m);
        }

        void operator()(const messages::SetHealth &k) {
            assert(k.playerId != connection.playerId);

            connection.announce(k);
        }

        void operator()(const messages::Log &l) {
            fmt::print("Log Request by [{:0>4}]: {}\n", connection.playerId, l.message);
        }
    } visitor { *this };

    std::visit(visitor, message);
}

void Connection::listenBody(const Container &container) {
    auto body = std::make_unique<uint8_t[]>(container.size);
    auto *ptr = body.get();

    auto respond = [this, container, body = std::move(body)](asio::error_code e, size_t n) {
        if (e == asio::error::eof || n != container.size) {
            markDisconnected();
            return;
        }

        handle(parse(container, body.get()));

        listen();
    };

    asio::async_read(socket, asio::buffer(ptr, container.size), std::move(respond));
}

void Connection::listen() {
    auto container = std::make_unique<Container>();
    auto *ptr = reinterpret_cast<uint8_t *>(container.get());

    auto respond = [this, container = std::move(container)](asio::error_code e, size_t n) {
        if (e == asio::error::eof || n != sizeof(Container)) {
            markDisconnected();
            return;
        }

        listenBody(*container);
    };

    asio::async_read(socket, asio::buffer(ptr, sizeof(Container)), std::move(respond));
}

Connection::Connection(int64_t playerId, tcp::socket socket, Server &server, messages::Hello hello)
    : playerId(playerId), server(server), socket(std::move(socket)), hello(std::move(hello)) {
    x = 0;
    y = 0;
}

void Server::accept(tcp::acceptor &acceptor) {
    temp = std::make_unique<tcp::socket>(context);

    acceptor.async_accept(*temp, [this, &acceptor](asio::error_code error) {
        {
            std::lock_guard guard(mutex);

            int64_t playerId = connections.size();
            connections.push_back(std::make_unique<Connection>(playerId, std::move(*temp), *this, messages::Hello {
                static_cast<int64_t>(connections.size()),
                lastTeam ? "red" : "blue"
            }));

            lastTeam = !lastTeam;

            fmt::print("New connection as [{:0>4}]\n", playerId);

            Connection &c = *connections[playerId];

//            auto buffer = std::make_unique<uint8_t[]>(sizeof(Verify));
//            auto ptr = buffer.get();
//
//            auto handle = [playerId, buffer = std::move(buffer)](asio::error_code error, size_t n) {
//                if (n != sizeof(Verify))
//                    throw std::exception();
//
//                auto *v = reinterpret_cast<Verify *>(buffer.get());
//                Verify k;
//
//                if (!(k == *v)) {
////                    throw std::exception();
//                } else {
//                    fmt::print("Player [{:0>4}] verified.\n", playerId);
//                }
//            };
//
//            asio::async_read(c.socket, asio::buffer(ptr, sizeof(Verify)), std::move(handle));

            c.write(c.hello);
            c.listen();

            for (const auto &e : connections) {
                if (e->playerId == playerId)
                    continue;

                e->write(messages::Move {
                    playerId, c.x, c.y, 0, 0, "idle", false
                });
            }
        }

        accept(acceptor);
    });
}

void Server::run() {
    tcp::acceptor acceptor(context, tcp::endpoint(tcp::v4(), options.port));

    accept(acceptor);

    fmt::print("Starting server on port {}.\n", options.port);
    context.run();
}

Server::Server(const Options &options) : options(options) { }
