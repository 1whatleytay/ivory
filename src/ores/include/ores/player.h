#pragma once

#include <engine/parts.h>

#include <ores/flag.h>
#include <ores/asset-loader.h>

struct Client;
struct Camera;

struct Player : public Child, public FlagHoldable {
    parts::BoxBody *body = nullptr;
    parts::BoxVisual *visual = nullptr;

    std::string color;

    Client *client = nullptr;
    Camera *camera = nullptr;

    bool flipX = false;
    const TagInfo *currentAnimation = nullptr;
    size_t currentFrame = 0;

    void setAnimation(const TagInfo &animation, bool flipX);

    TagInfo idle, walk, walkUp, walkDown;

    float visualWidth = 0, visualHeight = 0;

    bool left = false;

    std::vector<parts::TextureRange *> frames;

    float netUpdateTime = 0;

    std::pair<float, float> flagPosition() override;

    size_t netUpdateIndex = 0;
    static constexpr std::array netUpdates = { 0.05, 0.1, 0.2, 1.0, 0.0, 0.016 };

    float frameUpdateTime = 0;

    void doMovement();
    void doAnimation(float time);
    void doNetwork(float time);

    void update(float time) override;
    void keyboard(int key, int action) override;
    void click(int button, int action) override;

    explicit Player(Child *parent, std::string color, float x = 0, float y = 0.5);
};
