#pragma once

#include <engine/engine.h>

struct Camera : public Child {
    void update(float time) override;

    explicit Camera(Child *parent);
};
