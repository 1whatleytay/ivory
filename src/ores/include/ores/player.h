#pragma once

#include <engine/parts.h>

struct Player : public Child {
    parts::Box *box;

    [[nodiscard]] bool isTouchingGround() const;

    void update(float time) override;
    void keyboard(int key, int action) override;

    explicit Player(Child *parent, float x, float y);
};
