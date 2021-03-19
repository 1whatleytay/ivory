#include <shared/blocks.h>

#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>

#include <ctime>

BlockType::BlockType(std::string name, std::string path, bool solid)
    : name(std::move(name)), path(std::move(path)), solid(solid) { }

float ridgeNoise(float x, float y, unsigned char seed) {
    float frequency = 1.0f;
    float prev = 1.0f;
    float amplitude = 0.5f;
    float sum = 0.0f;

    for (int i = 0; i < 6; i++) {
        float r = stb_perlin_noise3_internal(x*frequency,y*frequency, 0, 0, 0, 0, (unsigned char)i + seed);
        r = 1.2 - (float) fabs(r);
        r = r*r;
        sum += r*amplitude*prev;
        prev = r;
        frequency *= 2.0;
        amplitude *= 0.5;
    }
    return sum;
}

namespace blocks {
    BlockType stone = { "Stone", "images/blocks/stone.png" };
    BlockType grass = { "Grass", "images/blocks/grass.png" };
    BlockType dirt = { "Dirt", "images/blocks/dirt.png" };
    BlockType goldOre = { "Gold Ore", "images/blocks/gold_ore.png" };
    BlockType diamondOre = { "Diamond Ore", "images/blocks/diamond_ore.png" };
    BlockType wood = { "Wood", "images/blocks/wood.png" };

    std::vector<const BlockType *> getBlocks() {
        return {
            &stone, &grass, &dirt, &goldOre, &diamondOre, &wood
        };
    };

    std::unordered_map<const BlockType *, BlockIndex> getIndices(const std::vector<const BlockType *> &blocks) {
        std::unordered_map<const BlockType *, BlockIndex> result;

        for (size_t a = 0; a < blocks.size(); a++)
            result[blocks[a]] = a;

        return result;
    }

    Blocks decode(const std::vector<BlockIndex> &blocks, const Blocks &index) {
        Blocks result(blocks.size());

        for (size_t a = 0; a < blocks.size(); a++)
            result[a] = blocks[a] < 0 ? nullptr : index[blocks[a]];

        return result;
    }

    std::vector<BlockIndex> encode(const Blocks &blocks, const Indices &index) {
        std::vector<BlockIndex> result(blocks.size());

        for (size_t a = 0; a < blocks.size(); a++)
            result[a] = blocks[a] ? index.at(blocks[a]) : -1;

        return result;
    }

    // Generation Constants
    constexpr size_t skyDepth = 20;
    constexpr size_t diamondDepth = 50;

    constexpr float surfaceCrazinessFactor = 7.0f;

    std::vector<const BlockType *> generate(size_t width, size_t height) {
        std::vector<const BlockType *> result(width * height);
        std::fill(result.begin(), result.end(), &stone);

        auto a = [width](size_t x, size_t y) { return x + y * width; };

        unsigned char seed = time(nullptr);

        for (size_t x = 0; x < width; x++) {
            for (size_t y = 0; y < height; y++) {
                auto v = 1 - ridgeNoise(x * 0.1, y * 0.1, seed);

                if (y < skyDepth && v + static_cast<float>(skyDepth - y) / surfaceCrazinessFactor > 1)
                    result[a(x, y)] = nullptr;

                if (result[a(x, y)] != &stone)
                    continue;

                if (v > 0.4f)
                    result[a(x, y)] = &goldOre;
                else if (v > 0.35f && y > diamondDepth)
                    result[a(x, y)] = &diamondOre;
            }
        }

        for (size_t x = 0; x < width; x++) {
            size_t y;
            for (y = 0; y < height; y++) {
                if (result[a(x, y)])
                    break;
            }

            if (y + 5 >= height)
                continue;

            result[a(x, y)] = &grass;

            for (size_t oy = 1; oy < 5; oy++)
                result[a(x, y + oy)] = &dirt;
        }

        return result;
    }
}
