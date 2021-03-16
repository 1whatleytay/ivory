#pragma once

#include <engine/engine.h>

#include <thread>

struct Client;

struct Game : public Child {
    Client *client = nullptr;
    std::unique_ptr<std::thread> clientThread;

    explicit Game(Engine &engine);
    ~Game();
};
