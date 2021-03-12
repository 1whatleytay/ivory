#pragma once

#include <engine/parts.h>

struct BlockType {
    std::string name;
    Color color;

    bool solid = true;

    BlockType(std::string name, Color color);
};

struct Block : public Child {
    const BlockType &type;

    parts::BoxVisual *visual;

    Block(Child *parent, const BlockType &type, float x, float y, float width, float height);
};

struct World : public Child {
    size_t width, height;

    std::vector<Block *> blocks;
    std::vector<Holder<parts::BoxBody>> bodies;

    void makeBodies();

    explicit World(Child *parent, size_t width = 40, size_t height = 100);
};
