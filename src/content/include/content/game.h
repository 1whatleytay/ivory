#pragma once

#include <content/net-player.h>

#include <engine/engine.h>

#include <thread>

struct Client;
struct Options;
struct Resources;

struct Game : public Child {
    Resources *resources = nullptr;
    std::unique_ptr<std::thread> clientThread;

    std::unordered_map<size_t, NetPlayer *> netPlayers;

    void update(float time) override;

    explicit Game(Engine &engine, const Options &options);
    ~Game();
};
