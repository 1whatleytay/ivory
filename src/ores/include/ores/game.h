#pragma once

#include <engine/engine.h>

struct Player;

struct Game : public Child {
    explicit Game(Engine &engine);
};
