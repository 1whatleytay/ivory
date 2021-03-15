#pragma once

#include <engine/engine.h>

struct Player;

struct Camera : public Child {
    bool usePlayer = true;
    Player *player = nullptr;

    void update(float time) override;
    void keyboard(int key, int action) override;

    explicit Camera(Child *parent);
};
