#pragma once

#include <engine/parts.h>

struct Player;

struct Flag : public Child {
    std::string color;

    parts::BoxVisual *visual = nullptr;
    parts::TextureRange *texture = nullptr;

    parts::BodyPtr body;
    std::any user;

    float spawnX = 0, spawnY = 0;

    void reset();

    Player *holdingPlayer = nullptr;

    void update(float time) override;

    Flag(Child *parent, std::string color, float x, float y);
};
