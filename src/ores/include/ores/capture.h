#pragma once

#include <engine/parts.h>

#include <any>

struct Capture : public Child {
    std::any user;

    std::string color;

    parts::BodyPtr body;

    Capture(Child *parent, std::string color, float x, float y);
};
