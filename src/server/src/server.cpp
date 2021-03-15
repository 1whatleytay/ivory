#include <server/server.h>

void Server::write(tcp::socket &socket, const Event &event) {
    Writer writer;
    event.write(writer);

    Container container;
    container.type = event.type();
    container.size = writer.data.size();

    asio::write(socket, asio::buffer(reinterpret_cast<const uint8_t *>(&container), sizeof(container)));
    asio::write(socket, asio::buffer(writer.data));
}

void Server::run() {
    tcp::acceptor acceptor(context, tcp::endpoint(tcp::v4(), 23788));

    while (true) {
        tcp::socket socket(context);
        acceptor.accept(socket);

        write(socket, Hello {
            0, 0.5f, 0.5f, options.worldWidth, options.worldHeight, blocks::encode(blocks, blockIndices)
        });
    }
}

Server::Server(const Options &options) : options(options) {
    blockList = blocks::getBlocks();
    blockIndices = blocks::getIndices();

    blocks = blocks::generate(options.worldWidth, options.worldHeight);
}
