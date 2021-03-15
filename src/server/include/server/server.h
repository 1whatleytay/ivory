#pragma once

#include <server/options.h>

#include <shared/blocks.h>
#include <shared/messages.h>

#include <asio.hpp>

#include <vector>

using asio::ip::tcp;

struct Server {
    asio::io_context context;

    const Options &options;

    blocks::Blocks blockList;
    blocks::Indices blockIndices;

    blocks::Blocks blocks;

    void write(tcp::socket &socket, const Event &event);

    [[noreturn]] void run();

    Server(const Options &options);
};
