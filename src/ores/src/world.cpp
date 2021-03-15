#include <ores/world.h>

#include <engine/parts.h>

#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <ctime>
#include <algorithm>

namespace blocks {
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

    BlockType stone = { "Stone", "images/stone.png" };
    BlockType grass = { "Grass", "images/grass.png" };
    BlockType dirt = { "Dirt", "images/dirt.png" };
    BlockType ore = { "Ore", "images/diamond_ore.png" };

    constexpr float blockSize = 1.2;

    std::vector<const BlockType *> generate(size_t width, size_t height) {
        std::vector<const BlockType *> result(width * height);
        std::fill(result.begin(), result.end(), &stone);

        auto a = [width](size_t x, size_t y) { return x + y * width; };

        unsigned char seed = time(nullptr);

        for (size_t x = 0; x < width; x++) {
            for (size_t y = 0; y < height; y++) {
                if (result[a(x, y)] != &stone)
                    continue;

//                auto v = 1 - stb_perlin_ridge_noise3(x * 0.1, y * 0.1, 0, 2.0, 0.5, 1.2, 6.0);
                auto v = 1 - ridgeNoise(x * 0.1, y * 0.1, seed);

                if (v > 0.4f)
                    result[a(x, y)] = &ore;
                else if (v > 0.2f)
                    result[a(x, y)] = &dirt;
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

BlockType::BlockType(std::string name, std::string path, bool solid)
    : name(std::move(name)), path(std::move(path)), solid(solid) { }

void Block::update(float time) {
    progress += time * 4;

    if (progress > 10 && isBreaking) {
        progress = 0;
        isBreaking = false;
        parent->as<World>().destroy.push_back(this);
    }

    float size = blocks::blockSize + isBreaking * std::sin(progress) * 0.2;

    visual->set(x, y, size, size, texture);
}

void Block::click(int button, int action) {
    if (action == GLFW_PRESS) {
        if (button == GLFW_MOUSE_BUTTON_1) {
            double kX = 0, kY = 0;
            glfwGetCursorPos(engine.window, &kX, &kY);

            int wW = 0, wH = 0;
            glfwGetWindowSize(engine.window, &wW, &wH);

            double cX = (2 * (kX / wW) - 1) * ((wW / engine.zoom) / 2) - engine.offsetX;
            double cY = -(2 * (kY / wH) - 1) * ((wH / engine.zoom) / 2) - engine.offsetY;

            if (cX > (x - blocks::blockSize / 2) && cX < (x + blocks::blockSize / 2)
                && cY > (y - blocks::blockSize / 2) && cY < (y + blocks::blockSize / 2)) {
                progress = 0;
                isBreaking = true;
            }
        }
    }
}

Block::Block(Child *parent, const BlockType &type, const parts::TextureRange &texture, float x, float y)
    : Child(parent), type(type), texture(texture), x(x), y(y) {

    visual = make<parts::BoxVisual>();
    visual->set(x, y, blocks::blockSize, blocks::blockSize, texture);
}

void World::update(float time) {
    if (!destroy.empty()) {
        for (const auto *d : destroy) {
            auto iterator = std::find(blocks.begin(), blocks.end(), d);
            assert(iterator != blocks.end());

            drop(*iterator);
        }

        destroy.clear();

        makeBodies();
    }
}

void World::makeBodies() {
    bodies.clear();

    auto a = [this](size_t x, size_t y) { return x + y * width; };

    std::vector<bool> taken(width * height);
    std::transform(blocks.begin(), blocks.end(), taken.begin(), [](const auto &e) { return !e || !e->type.solid; });

    for (int64_t y = 0; y < height; y++) {
        for (int64_t x = 0; x < width; x++) {
            int64_t w, h;

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

            for (size_t ow = 0; ow < w; ow++) {
                for (size_t oh = 0; oh < h; oh++) {
                    assert(!taken[a(x + ow, y + oh)]);

                    taken[a(x + ow, y + oh)] = true;
                }
            }

            constexpr float blockSize = 1.2;

            // only one block long and tall
            bodies.push_back(hold<parts::BoxBody>(
                x * blockSize + w * blockSize / 2 - blockSize / 2, -y * blockSize - h * blockSize / 2 + blockSize / 2,
                blockSize * w, blockSize * h));
        }
    }
}

World::World(Child *parent, size_t width, size_t height) : Child(parent), width(width), height(height) {
    supply<parts::Buffer>(width * height * 6);

    auto tex = get<parts::Texture>(100, 100);

    auto load = [&tex](const std::string &path) {
        int w, h, c;

        uint8_t *data = stbi_load(path.c_str(), &w, &h, &c, 4);

        parts::TextureRange range = tex->grab(w, h);
        range.write(data);

        stbi_image_free(data);

        return range;
    };

    auto a = [width](size_t x, size_t y) { return x + y * width; };

    auto data = blocks::generate(width, height);

    blocks.resize(width * height);

    for (int64_t x = 0; x < width; x++) {
        for (int64_t y = 0; y < height; y++) {
            const BlockType *type = data[a(x, y)];

            const parts::TextureRange *range;

            auto iterator = textures.find(type);
            if (iterator == textures.end()) {
                range = &textures.insert({ type, load(Engine::assets + type->path) }).first->second;
            } else {
                range = &iterator->second;
            }

            assert(range);

            blocks[a(x, y)] = make<Block>(*type, *range, x * blocks::blockSize, -y * blocks::blockSize);
        }
    }

    makeBodies();
}
