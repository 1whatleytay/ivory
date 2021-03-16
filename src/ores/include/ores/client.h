#pragma once

#include <engine/engine.h>

#include <shared/blocks.h>
#include <shared/messages.h>

#include <asio.hpp>

using asio::ip::tcp;

struct Client : public Resource {
    asio::io_context context;
    tcp::socket socket;

    messages::Hello hello;

    bool kill = false;

    Message read();
    void write(const Event &event);

    void run();

    Client(Child *component, const std::string &url, const std::string &port = "23788");
};
