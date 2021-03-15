#pragma once

#include <engine/parts.h>

#include <shared/blocks.h>

struct Block : public Child {
    const BlockType &type;
    const parts::TextureRange &texture;

    float x, y;

    parts::BoxVisual *visual;

    float progress = 0;
    bool isBreaking = false;

    void update(float time) override;
    void click(int button, int action) override;

    Block(Child *parent, const BlockType &type, const parts::TextureRange &texture, float x, float y);
};

struct World : public Child {
    size_t width, height;

    std::vector<Block *> blocks;
    std::vector<Holder<parts::BoxBody>> bodies;

    std::vector<Block *> destroy;

    std::unordered_map<const BlockType *, parts::TextureRange> textures;

    void update(float time) override;

    void makeBodies();

    explicit World(Child *parent, size_t width = 40, size_t height = 100);
};
