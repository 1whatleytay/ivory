#include <engine/shapes.h>

#include <fmt/printf.h>

Position::Position(float x, float y) : x(x), y(y) { }

Color::Color(uint32_t color) {
    red = (float)(color >> 16u) / 255.0f;
    green = (float)((color >> 8u) & 0xFFu) / 255.0f;
    blue = (float)(color & 0xFFu) / 255.0f;
}

void Vertex::mark() {
    glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(Vertex), (void *)offsetof(Vertex, position));
    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (void *)offsetof(Vertex, color));


    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

namespace shapes {
    std::array<Vertex, 6> square(float x, float y, float width, float height, const Color &color) {
        Vertex bottomLeft = { { x, y }, color };
        Vertex bottomRight = { { x + width, y }, color };
        Vertex topLeft = { { x, y + height }, color };
        Vertex topRight = { { x + width, y + height }, color };

        return {
            bottomRight, topLeft, bottomLeft,
            bottomRight, topRight, topLeft
        };
    }
}