#pragma once

#include <server/options.h>

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

    float x = 0, y = 0;

    bool disconnected = false;
    
    void markDisconnected();

    void write(const Event &event);
    void announce(const Event &event);

    void handle(const Message &message);

    void listenBody(const Container &container);
    void listen();

    Connection(size_t playerId, tcp::socket socket, Server &server, messages::Hello hello);
};

struct Server {
    std::mutex mutex;

    asio::io_context context;

    const Options &options;

    std::vector<std::unique_ptr<Connection>> connections;

    std::unique_ptr<tcp::socket> temp;

    bool lastTeam = false;

    void accept(tcp::acceptor &acceptor);

    void run();

    Server(const Options &options);
};
