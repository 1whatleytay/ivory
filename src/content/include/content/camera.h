#pragma once

#include <engine/engine.h>

struct Player;
struct FontText;
struct Resources;

struct Camera : public Child {
    Resources *resources = nullptr;

    bool usePlayer = true;

    int32_t redScore = 0, blueScore = 0;
    FontText *redScoreText = nullptr, *blueScoreText = nullptr;

    void update(float time) override;
    void keyboard(int key, int action) override;

    explicit Camera(Child *parent);
};
