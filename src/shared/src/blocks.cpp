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

    std::vector<const BlockType *> getBlocks() {
        return {
            &stone, &grass, &dirt, &goldOre, &diamondOre
        };
    };

    std::unordered_map<const BlockType *, int64_t> getIndices(const std::vector<const BlockType *> &blocks) {
        std::unordered_map<const BlockType *, int64_t> result;

        for (size_t a = 0; a < blocks.size(); a++)
            result[blocks[a]] = a;

        return result;
    }

    Blocks decode(const std::vector<int64_t> &blocks, const Blocks &index) {
        Blocks result(blocks.size());

        for (size_t a = 0; a < blocks.size(); a++)
            result[a] = a < 0 ? nullptr : index[blocks[a]];

        return result;
    }

    std::vector<int64_t> encode(const Blocks &blocks, const Indices &index) {
        std::vector<int64_t> result(blocks.size());

        for (size_t a = 0; a < blocks.size(); a++)
            result[a] = blocks[a] ? index.at(blocks[a]) : -1;

        return result;
    }

    std::vector<const BlockType *> generate(size_t width, size_t height) {
        std::vector<const BlockType *> result(width * height);
        std::fill(result.begin(), result.end(), &stone);

        auto a = [width](size_t x, size_t y) { return x + y * width; };

        unsigned char seed = time(nullptr);

        for (size_t x = 0; x < width; x++) {
            for (size_t y = 0; y < height; y++) {
                if (result[a(x, y)] != &stone)
                    continue;

                auto v = 1 - ridgeNoise(x * 0.1, y * 0.1, seed);

                if (v > 0.4f)
                    result[a(x, y)] = &goldOre;
                else if (v > 0.35f&& y > 30)
                    result[a(x, y)] = &diamondOre;
            }
        }

        for (size_t x = 0; x < width; x++) {
            result[a(x, 0)] = &grass;

            for (size_t y = 1; y < 5; y++)
                result[a(x, y)] = &dirt;
        }

        return result;
    }
}
