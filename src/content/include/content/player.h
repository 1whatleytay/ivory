#pragma once

#include <engine/parts.h>

#include <content/player-base.h>

struct Client;
struct Camera;

struct Player : public PlayerBase {
    float spawnX = 0, spawnY = 0;

    std::string color;

    Client *client = nullptr;
    Camera *camera = nullptr;

    bool isLocked = false, isPunching = false;

    float netUpdateTime = 0;

    float dieTimeout = 0.0;
    void die();

    size_t netUpdateIndex = 0;
    static constexpr std::array netUpdates = { 0.05, 0.1, 0.2, 1.0, 0.0, 0.016 };

    void revertAnimation();

    void doMovement();
    void doNetwork(float time);

    void update(float time) override;
    void click(int button, int action) override;
    void keyboard(int key, int action) override;

    explicit Player(Child *parent, std::string color, float x = 0, float y = 0.5);
};
