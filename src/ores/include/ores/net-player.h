#pragma once

#include <engine/parts.h>

#include <ores/flag.h>

struct Client;

struct NetPlayer : public Child, public FlagHoldable {
    float x, y;
    parts::BoxVisual *visual = nullptr;

    size_t playerId = 0;

    parts::TextureRange *range;

    bool hide = false;

    std::pair<float, float> flagPosition() override;

    void update(float time) override;

    NetPlayer(Child *parent, size_t playerId);
};
