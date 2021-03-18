#pragma once

#include <engine/parts.h>

struct Client;

struct Player : public Child {
    parts::Box *box = nullptr;

    Client *client = nullptr;

    [[nodiscard]] bool isTouchingGround() const;

    float netUpdateTime = 0;

    size_t netUpdateIndex = 0;
    static constexpr std::array netUpdates = { 0.05, 0.1, 0.2, 1.0, 0.0, 0.016 };

    void update(float time) override;
    void keyboard(int key, int action) override;

    explicit Player(Child *parent);
};
