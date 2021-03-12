#pragma once

#include <engine/handle.h>

#include <box2d/box2d.h>

#include <array>
#include <cstdint>

struct Position {
    float x = 0, y = 0;

    Position() = default;
    Position(float x, float y);
};

struct Color {
    float red = 0, green = 0, blue = 0;

    Color() = default;
    explicit Color(uint32_t color);
};

struct Vertex {
    Position position;
    Color color;

    static void mark();
};

namespace shapes {
    std::array<Vertex, 6> square(float x, float y, float width, float height, const Color &color);
}