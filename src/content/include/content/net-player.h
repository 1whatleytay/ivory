#pragma once

#include <engine/parts.h>

#include <shared/messages.h>

#include <content/flag.h>
#include <content/asset-loader.h>

struct Client;

struct NetPlayer : public Child, public FlagHoldable {
    parts::BoxBody *body = nullptr;
    parts::BoxVisual *visual = nullptr;

    Client *client = nullptr;

    size_t playerId = 0;

    bool flipX = false;
    const TagInfo *currentAnimation = nullptr;
    size_t currentFrame = 0;

    float frameUpdateTime = 0;

    int64_t clicks = 20;

    void setAnimation(const TagInfo &animation, bool flipX);

    TagInfo idle, walk, walkUp, walkDown;

    float visualWidth = 0, visualHeight = 0;

    bool left = false;

    std::vector<parts::TextureRange *> frames;

    bool hide = false;

    std::pair<float, float> flagPosition() override;

    void handle(const messages::Move &event);

    void click(int button, int action) override;

    void update(float time) override;

    NetPlayer(Child *parent, size_t playerId, float x, float y);
};
