#pragma once

#include <engine/parts.h>

struct Client;

struct Player : public Child {
    parts::Box *box = nullptr;

    Client *client = nullptr;

    [[nodiscard]] bool isTouchingGround() const;

    void update(float time) override;
    void keyboard(int key, int action) override;

    explicit Player(Child *parent);
};
