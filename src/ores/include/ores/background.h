#pragma once

#include <engine/parts.h>

struct Background : public Child {
    parts::TextureRange *range;

    parts::BoxVisual *visual;

    void draw() override;
    void update(float time) override;

    Background(Child *parent);
};
