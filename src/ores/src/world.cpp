#include <ores/world.h>

#include <engine/parts.h>

#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>

#include <fmt/printf.h>

BlockType::BlockType(std::string name, Color color) : name(std::move(name)), color(color) { }

Block::Block(Child *parent, const BlockType &type, float x, float y, float width, float height)
    : Child(parent), type(type) {
    visual = make<parts::BoxVisual>();
    visual->set(x, y, width, height, type.color);
}

namespace blocks {
    BlockType stone = { "Stone", Color(0xA3A19B) };
    BlockType grass = { "Grass", Color(0xD0E831) };
    BlockType dirt = { "Dirt", Color(0xBF8B2A) };
    BlockType ore = { "Ore", Color(0x2E88F0) };
    BlockType fuel = { "Fuel", Color(0xD94EA4) };
    BlockType gem = { "Gem", Color(0x5D00FF) };

    std::vector<BlockType *> generate(size_t width, size_t height) {
        std::vector<BlockType *> blocks(width * height);
        std::fill(blocks.begin(), blocks.end(), &stone);

        auto a = [width](size_t x, size_t y) { return x + y * width; };

        for (size_t x = 0; x < width; x++) {
            blocks[a(x, 0)] = &grass;

            for (size_t y = 1; y < 5; y++)
                blocks[a(x, y)] = &dirt;
        }

        return blocks;
    }
}

void World::makeBodies() {
    bodies.clear();

    auto a = [this](size_t x, size_t y) { return x + y * width; };

    std::vector<bool> taken(width * height);
    std::transform(blocks.begin(), blocks.end(), taken.begin(), [](const auto &e) { return !e->type.solid; });

    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            size_t w, h;

            // prelim to determine width
            for (w = 0; x + w < width; w++) {
                if (taken[a(x + w, y)])
                    break;
            }

            if (w < 1)
                continue;

            // prelim to determine height
            for (h = 1; y + h < height; h++) {
                bool done = false;

                for (size_t ow = 0; ow < w; ow++) {
                    if (taken[a(x + ow, y + h)]) {
                        done = true;
                        break;
                    }
                }

                if (done)
                    break;
            }

            fmt::print("Block {}, {} -> {}, {}\n", x, y, w, h);

            for (size_t ow = 0; ow < width; ow++) {
                for (size_t oh = 0; oh < height; oh++) {
                    assert(!taken[a(x + ow, y + oh)]);

                    taken[a(x + ow, y + oh)] = true;
                }
            }

            constexpr float blockSize = 1.2;

            // only one block long and tall
            bodies.push_back(hold<parts::BoxBody>(x * blockSize, -y * blockSize, blockSize, blockSize));

            x += w;
        }
    }
}

World::World(Child *parent, size_t width, size_t height) : Child(parent), width(width), height(height) {
    create<parts::Buffer>(width * height * 6);

    auto a = [width](size_t x, size_t y) { return x + y * width; };

    auto data = blocks::generate(width, height);

    blocks.resize(width * height);

    constexpr float blockSize = 1.2;

    for (int64_t x = 0; x < width; x++) {
        for (int64_t y = 0; y < height; y++) {
            blocks[a(x, y)] = make<Block>(*data[a(x, y)], x * blockSize, -y * blockSize, blockSize, blockSize);
        }
    }

    makeBodies();
}
