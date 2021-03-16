#pragma once

#include <server/options.h>

#include <shared/blocks.h>
#include <shared/messages.h>

#include <asio.hpp>

#include <mutex>
#include <vector>

using asio::ip::tcp;

struct Server;

struct Connection {
    std::mutex mutex;

    size_t playerId;

    Server &server;
    messages::Hello hello;

    tcp::socket socket;

    size_t bytes = 0;
    std::vector<uint8_t> buffer;

    void write(const Event &event);
    void announce(const Event &event);

    void check();
    void listen();
    void handle(const Message &message);

    Connection(size_t playerId, tcp::socket socket, Server &server, messages::Hello hello);
};

struct Server {
    std::mutex mutex;

    asio::io_context context;

    const Options &options;

    blocks::Blocks blockList;
    blocks::Indices blockIndices;

    blocks::Blocks blocks;

    std::vector<std::unique_ptr<Connection>> connections;

    std::unique_ptr<tcp::socket> temp;

    void accept(tcp::acceptor &acceptor);

    void run();

    Server(const Options &options);
};
