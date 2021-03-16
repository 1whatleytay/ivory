#pragma once

#include <engine/parts.h>

#include <shared/blocks.h>

struct Client;

struct Block : public Child {
    const BlockType &type;
    const parts::TextureRange &texture;

    size_t posX = 0, posY = 0;
    float x = 0, y = 0;

    parts::BoxVisual *visual;

    float progress = 0;
    bool isBreaking = false;

    void update(float time) override;
    void click(int button, int action) override;

    Block(Child *parent, const BlockType &type, const parts::TextureRange &texture,
        size_t posX, size_t posY, float x, float y);
};

struct World : public Child {
    size_t width = 0, height = 0;

    Client *client = nullptr;

    std::vector<Block *> blocks;
    std::vector<Holder<parts::BoxBody>> bodies;

    std::vector<Block *> destroy;

    std::unordered_map<const BlockType *, parts::TextureRange> textures;

    void update(float time) override;

    void makeBodies();

    explicit World(Child *parent);
};
