#pragma once

#include <engine/parts.h>

#include <shared/messages.h>

#include <content/flag.h>
#include <content/asset-loader.h>

enum class Direction {
    Up, Down, Left, Right
};

Direction selectDirection(const Position &object, const Position &towards);
Direction selectLinearDirection(int x, int y);

struct DirectionTags {
    TagInfo up, down, left, right;

    const TagInfo &get(Direction direction) const;

    DirectionTags() = default;
    DirectionTags(TagInfo up, TagInfo down, TagInfo left, TagInfo right);
    DirectionTags(const AssetLoader &loader, const std::string &base);
    DirectionTags(const AssetLoader &loader, const std::string &base, const std::string &stage);
};

struct PunchTags {
    DirectionTags start, windup, recoil, end;

    PunchTags() = default;
    PunchTags(const AssetLoader &loader, const std::string &name);
};

struct PlayerBase : public Child, public FlagHoldable {
    parts::BoxBody *body = nullptr;
    parts::BoxVisual *visual = nullptr;

    float visualWidth = 0, visualHeight = 0;

    size_t currentFrame = 0;
    const TagInfo *currentAnimation = nullptr;
    std::optional<std::function<void()>> onFinishAnimation;

    static constexpr float frameDuration = 0.1;

    float frameUpdateTime = 0;

    Direction direction = Direction::Right;

    // Custom Animations
    DirectionTags idle, walk;
    PunchTags punch;

    std::vector<parts::TextureRange *> frames;

    Position flagPosition() override;

    void update(float time) override;

    void doAnimation(const TagInfo &animation, std::optional<std::function<void()>> onFinish = std::nullopt);

    void pickAnimation(const DirectionTags &tags, std::optional<std::function<void()>> onFinish = std::nullopt);
    void pickAnimation(const DirectionTags &tags, Direction direction, std::optional<std::function<void()>> onFinish = std::nullopt);

    PlayerBase(Child *child, float x, float y);
};
