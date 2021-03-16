#include <ores/world.h>

#include <ores/assets.h>
#include <ores/client.h>

#include <ctime>
#include <algorithm>

void Block::update(float time) {
    progress += time * 4;

    if (progress > 2 && isBreaking) {
        progress = 0;
        isBreaking = false;
        parent->as<World>().destroy.push_back(this);
    }

    float size = blocks::blockSize + isBreaking * std::sin(progress) * 0.2;

    visual->set(x, y, size, size, texture, isBreaking ? -0.1 : 0);
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

Block::Block(Child *parent, const BlockType &type, const parts::TextureRange &texture,
    size_t posX, size_t posY, float x, float y)
    : Child(parent), type(type), texture(texture), posX(posX), posY(posY), x(x), y(y) {

    visual = make<parts::BoxVisual>();
    visual->set(x, y, blocks::blockSize, blocks::blockSize, texture);
}

void World::update(float time) {
    if (!destroy.empty()) {
        for (const auto *d : destroy) {
            if (client) {
                client->write(messages::Replace {
                    d->posX, d->posY, -1
                });
            }

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

World::World(Child *parent) : Child(parent), width(40), height(100) {
    client = find<Client>();

    if (client) {
        width = client->hello.worldWidth;
        height = client->hello.worldHeight;
    }

    supply<parts::Buffer>(width * height * 6);

    auto a = [this](size_t x, size_t y) { return x + y * width; };

    auto data = client ? blocks::decode(client->hello.blocks, blocks::getBlocks()) : blocks::generate(width, height);
    auto tex = get<parts::Texture>(100, 100);

    blocks.resize(width * height);

    for (int64_t x = 0; x < width; x++) {
        for (int64_t y = 0; y < height; y++) {
            const BlockType *type = data[a(x, y)];

            if (!type)
                continue;

            const parts::TextureRange *range;

            auto iterator = textures.find(type);
            if (iterator == textures.end()) {
                range = &textures.insert({ type, assets::load(*tex, type->path) }).first->second;
            } else {
                range = &iterator->second;
            }

            assert(range);

            blocks[a(x, y)] = make<Block>(*type, *range, x, y, x * blocks::blockSize, -y * blocks::blockSize);
        }
    }

    makeBodies();
}
