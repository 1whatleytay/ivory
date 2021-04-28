#pragma once

#include <engine/parts.h>

struct Flag;

struct FlagHoldable {
    Flag *holding = nullptr;

    virtual std::pair<float, float> flagPosition() = 0;

    virtual ~FlagHoldable() = default;
};

struct Flag : public Child {
    std::string color;

    parts::BoxVisual *visual = nullptr;
    parts::TextureRange *texture = nullptr;

    parts::BodyPtr body;
    std::any user;

    float spawnX = 0, spawnY = 0;

    FlagHoldable *holding = nullptr;

    void reset();
    void pickUp(FlagHoldable *by);

    void update(float time) override;

    Flag(Child *parent, std::string color, float x, float y);
};
