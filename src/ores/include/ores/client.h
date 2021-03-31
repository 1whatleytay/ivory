#pragma once

#include <engine/engine.h>

#include <shared/messages.h>

#include <asio.hpp>

#include <condition_variable>

using asio::ip::tcp;

struct Client : public Resource {
    std::mutex mutex;

    asio::io_context context;
    tcp::socket socket;

    messages::Hello hello;
    volatile bool hasHello = false;

    std::mutex messagesMutex;
    std::vector<Message> messages;

    void write(const Event &event);

    void listen();
    void listenBody(const Container &container);

    void handle(const Message &message);

    void run();

    Client(Child *component, const std::string &url, const std::string &port = "23788");
};
