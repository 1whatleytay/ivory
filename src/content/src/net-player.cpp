#include <content/net-player.h>

#include <content/client.h>

#include <fmt/printf.h>

#include <unordered_map>

// A lot of duplicate code here, because I want to sync with player as much as possible.
// Will probably have to extend stuff later.

std::pair<float, float> NetPlayer::flagPosition() {
    auto position = body->value->GetPosition();

    return { position.x, position.y + visualHeight * 1.5f };
}

void NetPlayer::setAnimation(const TagInfo &animation, bool flip) {
    if (&animation != currentAnimation || flip != flipX) {
        currentAnimation = &animation;
        currentFrame = 0;
    }

    flipX = flip;
}

void NetPlayer::handle(const messages::Move &event) {
    body->value->SetTransform(b2Vec2(event.x, event.y), 0);
    body->value->SetLinearVelocity(b2Vec2(event.velocityX, event.velocityY));
    body->value->SetAwake(true);

//    fmt::print("{}, {}, -> {}, {}\n", event.x, event.y, event.velocityX, event.velocityY);

    std::unordered_map<std::string, TagInfo *> tags;

    auto add = [&tags](TagInfo &tag) {
        tags.insert({ tag.name, &tag });
    };

    add(idle);
    add(walk);
    add(walkUp);
    add(walkDown);

    auto iterator = tags.find(event.animation);
    if (iterator != tags.end())
        setAnimation(*iterator->second, event.flipX);
}

void NetPlayer::click(int button, int action) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double kX = 0, kY = 0;
        glfwGetCursorPos(engine.window, &kX, &kY);

        int wW = 0, wH = 0;
        glfwGetWindowSize(engine.window, &wW, &wH);

        double cX = (2 * (kX / wW) - 1) * ((wW / engine.zoom) / 2) - engine.offsetX;
        double cY = -(2 * (kY / wH) - 1) * ((wH / engine.zoom) / 2) - engine.offsetY;

        auto pos = body->value->GetPosition();

        if (cX >= pos.x - visualWidth / 2 && cX <= pos.x + visualWidth / 2
            && cY >= pos.y && cY <= pos.y + visualHeight) {
            clicks--;

            fmt::print("Clicking: {}, clicks left: {}\n", playerId, clicks);

            if (clicks <= 0) {
                client->write(messages::SetHealth {
                    playerId,

                    true
                });

                clicks = 20;
            }
        }
    }
}

void NetPlayer::update(float time) {
    constexpr float frameDuration = 0.1;
    frameUpdateTime += time;

    while (frameUpdateTime > frameDuration) {
        currentFrame++;
        frameUpdateTime -= frameDuration;
    }

    size_t i = currentFrame % (currentAnimation->end - currentAnimation->start + 1) + currentAnimation->start;

    auto pos = body->value->GetPosition();

    visual->set(pos.x, pos.y + visualHeight / 2, visualWidth, visualHeight, *frames[i], flipX);
}

NetPlayer::NetPlayer(Child *parent, int64_t playerId, float x, float y) : Child(parent), playerId(playerId) {
    client = find<Client>();

    AssetLoader loader((engine.assets / "images/players/nate.json").string(), engine.assets.string());

    auto size = loader.size();

    auto tex = assemble<parts::Texture>(loader.image.width, loader.image.height, loader.image.data.get());
    frames.resize(loader.frames.size());
    for (size_t a = 0; a < loader.frames.size(); a++) {
        const FrameInfo &info = loader.frames[a];

        frames[a] = tex->grab(info.x, info.y, info.width, info.height);
    }

    idle = loader.tags.at("idle");
    walk = loader.tags.at("walk");
    walkUp = loader.tags.at("walk up");
    walkDown = loader.tags.at("walk down");

    setAnimation(idle, false);

    constexpr float targetHeight = 0.75;

    visualHeight = targetHeight;
    visualWidth = targetHeight / size.second * size.first;

    visual = make<parts::BoxVisual>();
    body = make<parts::BoxBody>(x, y, visualWidth, visualHeight / 2, 1);
}
