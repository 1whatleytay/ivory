#include <engine/vertex.h>

Vec2::Vec2(float x, float y) : x(x), y(y) { }

std::array<uint8_t, 4> Color::data() const {
    return {
        static_cast<uint8_t>(red * 255.0f),
        static_cast<uint8_t>(green * 255.0f),
        static_cast<uint8_t>(blue * 255.0f),
        255
    };
}

Color::Color(uint32_t color) {
    red = (float)(color >> 16u) / 255.0f;
    green = (float)((color >> 8u) & 0xFFu) / 255.0f;
    blue = (float)(color & 0xFFu) / 255.0f;
}

void Vertex::mark() {
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (void *)offsetof(Vertex, position));
    glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Vertex), (void *)offsetof(Vertex, texCoord));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}
