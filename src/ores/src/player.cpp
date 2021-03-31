#include <ores/player.h>

#include <ores/flag.h>
#include <ores/client.h>
#include <ores/asset-loader.h>

#include <fmt/printf.h>

void Player::setAnimation(const TagInfo &animation, bool flip) {
    if (&animation != currentAnimation || flip != flipX) {
        currentAnimation = &animation;
        currentFrame = 0;
    }

    flipX = flip;
}

void Player::update(float time) {
    float x = glfwGetKey(engine.window, GLFW_KEY_D) - glfwGetKey(engine.window, GLFW_KEY_A);
    float y = glfwGetKey(engine.window, GLFW_KEY_W) - glfwGetKey(engine.window, GLFW_KEY_S);

    constexpr float playerSpeed = 4.5;

    b2Vec2 velo(x, y);
    velo.Normalize();
    velo *= playerSpeed;

    body->value->SetLinearVelocity(velo);

    constexpr float frameDuration = 0.1;
    frameUpdateTime += time;

    while (frameUpdateTime > frameDuration) {
        currentFrame++;
        frameUpdateTime -= frameDuration;
    }

    size_t i = currentFrame % (currentAnimation->end - currentAnimation->start + 1) + currentAnimation->start;

    auto pos = body->value->GetPosition();

    visual->set(pos.x, pos.y + visualHeight / 2, visualWidth, visualHeight, *frames[i], -0.13, flipX);

    if (holdingFlag)
        holdingFlag->body->SetTransform(b2Vec2(pos.x, pos.y + visualHeight * 1.5f), 0);

    if (client) {
        float timeToGo = netUpdates[netUpdateIndex % netUpdates.size()];

        netUpdateTime += time;
        if (netUpdateTime > timeToGo) {
            if (timeToGo == 0) {
                netUpdateTime = 0;
            } else {
                netUpdateTime = std::fmod(netUpdateTime, timeToGo);
            }

            client->write(messages::Move {
                client->hello.playerId,

                pos.x, pos.y
            });
        }
    }
}

void Player::keyboard(int key, int action) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_R) {
            float x = client ? client->hello.playerX : 0;
            float y = client ? client->hello.playerY : 2;

            body->value->SetTransform(b2Vec2(x, y), 0);
            body->value->SetAwake(true);
        }

        if (key == GLFW_KEY_U) {
            netUpdateIndex++;
        }

        if (key == GLFW_KEY_A)
            left = true;
        if (key == GLFW_KEY_D)
            left = false;

        // Interact
        if (key == GLFW_KEY_F) {
            b2ContactEdge *edge = body->value->GetContactList();

            if (holdingFlag) {
                holdingFlag->holdingPlayer = nullptr;
                holdingFlag = nullptr;
            } else {
                while (edge) {
                    std::any *r = reinterpret_cast<std::any *>(edge->other->GetUserData().pointer);

                    if (r && r->has_value() && r->type() == typeid(Flag *)) {
                        Flag *flag = std::any_cast<Flag *>(*r);

                        if (flag && !holdingFlag && !flag->holdingPlayer) {
                            holdingFlag = flag;
                            flag->holdingPlayer = this;
                        }
                    }

                    edge = edge->next;
                }
            }
        }
    }

    auto isMovementKey = [key]() {
        return key == GLFW_KEY_A
            || key == GLFW_KEY_D
            || key == GLFW_KEY_W
            || key == GLFW_KEY_S;
    };

    if (action != GLFW_REPEAT && isMovementKey()) {
        int x = glfwGetKey(engine.window, GLFW_KEY_D) - glfwGetKey(engine.window, GLFW_KEY_A);
        int y = glfwGetKey(engine.window, GLFW_KEY_W) - glfwGetKey(engine.window, GLFW_KEY_S);

        if (x > 0)
            setAnimation(walk, false);
        else if (x < 0)
            setAnimation(walk, true);
        else if (y > 0)
            setAnimation(walkUp, false);
        else if (y < 0)
            setAnimation(walkDown, false);
        else if (left)
            setAnimation(idle, true);
        else
            setAnimation(idle, false);
    }
}

void Player::click(int button, int action) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double kX = 0, kY = 0;
        glfwGetCursorPos(engine.window, &kX, &kY);

        int wW = 0, wH = 0;
        glfwGetWindowSize(engine.window, &wW, &wH);

        double cX = (2 * (kX / wW) - 1) * ((wW / engine.zoom) / 2) - engine.offsetX;
        double cY = -(2 * (kY / wH) - 1) * ((wH / engine.zoom) / 2) - engine.offsetY;

        body->value->SetTransform(b2Vec2(cX, cY), 0);
    }
}

Player::Player(Child *parent, float x, float y) : Child(parent) {
    client = find<Client>();

    if (client) {
        x = client->hello.playerX;
        y = client->hello.playerY;
    }

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
