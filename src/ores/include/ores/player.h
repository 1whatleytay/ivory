#pragma once

#include <engine/parts.h>

#include <ores/asset-loader.h>

struct Client;

struct Player : public Child {
    parts::Box *box = nullptr;

    Client *client = nullptr;

    const TagInfo *currentAnimation = nullptr;
    size_t currentFrame = 0;

    void setAnimation(const TagInfo &animation);

    TagInfo idle;

    std::vector<parts::TextureRange *> frames;

    float netUpdateTime = 0;

    size_t netUpdateIndex = 0;
    static constexpr std::array netUpdates = { 0.05, 0.1, 0.2, 1.0, 0.0, 0.016 };

    float frameUpdateTime = 0;

    void update(float time) override;
    void keyboard(int key, int action) override;
    void click(int button, int action) override;

    explicit Player(Child *parent);
};
