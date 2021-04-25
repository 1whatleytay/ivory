#pragma once

#include <engine/engine.h>

struct Player;
struct FontText;
struct Resources;

struct Camera : public Child {
    Resources *resources = nullptr;

    bool usePlayer = true;

    int32_t leftScore = 0, rightScore = 0;
    FontText *leftScoreText = nullptr, *rightScoreText = nullptr;

    void update(float time) override;
    void keyboard(int key, int action) override;

    explicit Camera(Child *parent);
};
