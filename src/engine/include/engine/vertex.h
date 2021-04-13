#pragma once

#include <engine/handle.h>

#include <box2d/box2d.h>

#include <array>
#include <cstdint>

struct Vec2 {
    float x = 0, y = 0;

    Vec2() = default;
    Vec2(float x, float y);
};

struct Color {
    float red = 0, green = 0, blue = 0;

    std::array<uint8_t, 4> data() const;

    Color() = default;
    explicit Color(uint32_t color);
};

struct Vertex {
    Vec2 position;
    Vec2 texCoord;

    static void mark();
};
