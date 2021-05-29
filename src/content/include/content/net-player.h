#pragma once

#include <engine/parts.h>

#include <shared/messages.h>

#include <content/flag.h>
#include <content/player-base.h>

struct Client;

struct NetPlayer : public PlayerBase {
    Client *client = nullptr;

    int64_t playerId = 0;

    int64_t clicks = 20;

    bool left = false;

    bool hide = false;

    void handle(const messages::Move &event);

    void click(int button, int action) override;

    NetPlayer(Child *parent, int64_t playerId, float x, float y);
};
