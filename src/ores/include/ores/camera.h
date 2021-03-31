#pragma once

#include <engine/engine.h>

struct Player;
struct Resources;

struct Camera : public Child {
    Resources *resources = nullptr;

    bool usePlayer = true;

    void update(float time) override;
    void keyboard(int key, int action) override;

    explicit Camera(Child *parent);
};
