#include <content/player-base.h>

Direction selectDirection(const Position &object, const Position &towards) {
    Position difference = { towards.x - object.x, towards.y - object.y };

    if (std::abs(difference.x) < std::abs(difference.y)) {
        return difference.y > 0 ? Direction::Up : Direction::Down;
    } else {
        return difference.x > 0 ? Direction::Right : Direction::Left;
    }
}

Direction selectLinearDirection(int x, int y) {
    if (x > 0)
        return Direction::Right;
    else if (x < 0)
        return Direction::Left;
    else if (y > 0)
        return Direction::Up;
    else if (y < 0)
        return Direction::Down;

    assert(false);
}

const TagInfo &DirectionTags::get(Direction direction) const {
    switch (direction) {
        case Direction::Up: return up;
        case Direction::Down: return down;
        case Direction::Left: return left;
        case Direction::Right: return right;
        default: assert(false);
    }
}

DirectionTags::DirectionTags(TagInfo up, TagInfo down, TagInfo left, TagInfo right)
    : up(std::move(up)), down(std::move(down)), left(std::move(left)), right(std::move(right)) { }

DirectionTags::DirectionTags(const AssetLoader &loader, const std::string &base) {
    auto rightPos = fmt::format("{} right", base), leftPos = fmt::format("{} left", base);
    auto upPos = fmt::format("{} up", base), downPos = fmt::format("{} down", base);

    up = loader.findTag(upPos);
    down = loader.findTag(downPos);
    left = loader.findTag(leftPos);
    right = loader.findTag(rightPos);
}

DirectionTags::DirectionTags(const AssetLoader &loader, const std::string &base, const std::string &stage) {
    auto rightPos = fmt::format("{} right", base), leftPos = fmt::format("{} left", base);
    auto upPos = fmt::format("{} up", base), downPos = fmt::format("{} down", base);

    up = loader.findTagIn(stage, upPos);
    down = loader.findTagIn(stage, downPos);
    left = loader.findTagIn(stage, leftPos);
    right = loader.findTagIn(stage, rightPos);
}

PunchTags::PunchTags(const AssetLoader &loader, const std::string &name) {
    start = DirectionTags(loader, name, "start");
    windup = DirectionTags(loader, name, "windup");
    recoil = DirectionTags(loader, name, "recoil");
    end = DirectionTags(loader, name, "end");
}

Position PlayerBase::flagPosition() {
    auto position = body->value->GetPosition();

    return { position.x, position.y + visualHeight * 1.5f };
}

void PlayerBase::doAnimation(const TagInfo &animation, std::optional<std::function<void()>> onFinish) {
    if (&animation != currentAnimation) {
        currentAnimation = &animation;
        currentFrame = 0;
    }

    onFinishAnimation = std::move(onFinish);
}

void PlayerBase::pickAnimation(const DirectionTags &tags, Direction dir, std::optional<std::function<void()>> onFinish) {
    doAnimation(tags.get(dir), std::move(onFinish));
}

void PlayerBase::pickAnimation(const DirectionTags &tags, std::optional<std::function<void()>> onFinish) {
    pickAnimation(tags, direction, std::move(onFinish));
}

void PlayerBase::update(float time) {
    frameUpdateTime += time;

    while (frameUpdateTime > frameDuration) {
        currentFrame++;
        frameUpdateTime -= frameDuration;
    }

    while (currentFrame + currentAnimation->start > currentAnimation->end) {
        currentFrame -= currentAnimation->end - currentAnimation->start + 1;

        if (onFinishAnimation) {
            // rough
            auto f = std::move(onFinishAnimation.value());
            onFinishAnimation.reset();

            f();
        }
    }

    size_t i = currentFrame + currentAnimation->start;

    auto pos = body->value->GetPosition();

    visual->set(pos.x, pos.y + visualHeight / 2, visualWidth, visualHeight, *frames[i]);
}

PlayerBase::PlayerBase(Child *child, float x, float y) : Child(child) {
    AssetLoader loader((engine.assets / "images/players/nate1.json").string(), engine.assets.string());

    auto size = loader.size();

    auto tex = assemble<parts::Texture>(loader.image.width, loader.image.height, loader.image.data.get());
    frames.resize(loader.frames.size());
    for (size_t a = 0; a < loader.frames.size(); a++) {
        const FrameInfo &info = loader.frames[a];

        frames[a] = tex->grab(info.x, info.y, info.width, info.height);
    }

    idle = {
        loader.findTag("idle up"),
        loader.findTag("idle down"),
        loader.findTag("idle down"),
        loader.findTag("idle up"),
    };
    walk = DirectionTags(loader, "walk");
    punch = PunchTags(loader, "attack 1");

    doAnimation(idle.left);

    constexpr float targetHeight = 0.75;
    constexpr float scale = 1.25;

    visualHeight = targetHeight * scale;
    visualWidth = targetHeight / size.second * size.first * scale;

    constexpr float collisionRatio = 8 / 23.0f;

    visual = make<parts::BoxVisual>();
    body = make<parts::BoxBody>(x, y, visualHeight * collisionRatio, visualHeight / 2, 1);
}
