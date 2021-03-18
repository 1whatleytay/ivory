#pragma once

#include <engine/parts.h>

#include <mutex>

struct Client;

struct NetPlayer : public Child {
    float x, y;
    parts::BoxVisual *visual = nullptr;

    size_t playerId = 0;

    parts::TextureRange *range;

    bool hide = false;

    void update(float time) override;

    NetPlayer(Child *parent, size_t playerId);
};
