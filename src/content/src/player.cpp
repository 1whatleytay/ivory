#include <content/player.h>

#include <content/camera.h>
#include <content/client.h>
#include <content/capture.h>
#include <content/resources.h>

#include <fmt/printf.h>

void Player::setAnimation(const TagInfo &animation, bool flip) {
    if (&animation != currentAnimation || flip != flipX) {
        currentAnimation = &animation;
        currentFrame = 0;
    }

    flipX = flip;
}

void Player::doMovement() {
    if (dieTimeout > 0)
        return;

    float x = glfwGetKey(engine.window, GLFW_KEY_D) - glfwGetKey(engine.window, GLFW_KEY_A);
    float y = glfwGetKey(engine.window, GLFW_KEY_W) - glfwGetKey(engine.window, GLFW_KEY_S);

    float playerSpeed = 3;

    if (holding) {
        if (holding->color == color)
            playerSpeed = 2;
        else
            playerSpeed = 2.5;
    }

    b2Vec2 velo(x, y);
    velo.Normalize();
    velo *= playerSpeed;

    body->value->SetLinearVelocity(velo);
}

void Player::doAnimation(float time) {
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

void Player::doNetwork(float time) {
    if (!client)
        return;
    float timeToGo = netUpdates[netUpdateIndex % netUpdates.size()];

    auto pos = body->value->GetPosition();
    auto velocity = body->value->GetLinearVelocity();

    netUpdateTime += time;
    if (netUpdateTime > timeToGo) {
        if (timeToGo == 0) {
            netUpdateTime = 0;
        } else {
            netUpdateTime = std::fmod(netUpdateTime, timeToGo);
        }

        client->write(messages::Move {
            client->hello.playerId,

            pos.x, pos.y,
            velocity.x, velocity.y,

            currentAnimation->name, flipX
        });
    }
}

void Player::update(float time) {
    doMovement();
    doAnimation(time);
    doNetwork(time);

    dieTimeout -= time;

    b2ContactEdge *edge = body->value->GetContactList();

    while (edge) {
        auto *r = reinterpret_cast<std::any *>(edge->other->GetUserData().pointer);

        if (r && r->has_value() && r->type() == typeid(Capture *)) {
            auto *point = std::any_cast<Capture *>(*r);

            // If player is holding a flag and the point is the player's point and
            if (point && holding && holding->color != color && point->color == color) {
                std::string flagColor = holding->color;

                holding->reset(); // this will holdingFlag = nullptr for me :flushed:

                if (client)
                    client->write(messages::Capture { std::move(flagColor) });

                if (color == "red") {
                    camera->redScore++;
                } else if (color == "blue") {
                    camera->blueScore++;
                }
            }
        }

        edge = edge->next;
    }
}

void Player::keyboard(int key, int action) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_U) {
            netUpdateIndex++;
        }

        if (key == GLFW_KEY_A)
            left = true;
        if (key == GLFW_KEY_D)
            left = false;

        // Interact
        if (key == GLFW_KEY_F) {
            if (holding) {
                if (client) {
                    Flag *flag = holding;

                    holding->pickUp(nullptr);

                    auto position = flag->body->GetPosition();

                    client->write(messages::PickUp {
                        true, client->hello.playerId, flag->color,
                        position.x, position.y
                    });
                }
            } else {
                b2ContactEdge *edge = body->value->GetContactList();

                while (edge) {
                    auto *r = reinterpret_cast<std::any *>(edge->other->GetUserData().pointer);

                    if (r && r->has_value() && r->type() == typeid(Flag *)) {
                        Flag *flag = std::any_cast<Flag *>(*r);

                        if (flag && !holding && !flag->holding) {
                            flag->pickUp(this);

                            if (client) {
                                auto pos = flagPosition();

                                client->write(messages::PickUp {
                                    false, client->hello.playerId, flag->color,
                                    pos.first, pos.second
                                });
                            }
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

void Player::die() {
    dieTimeout = 5;

    if (holding) {
        Flag *flag = holding;

        holding->pickUp(nullptr);

        auto position = flag->body->GetPosition();

        client->write(messages::PickUp {
            true, client->hello.playerId, flag->color,
            position.x, position.y
        });
    }

    body->value->SetTransform(b2Vec2(spawnX, spawnY), 0);
}

std::pair<float, float> Player::flagPosition() {
    auto pos = body->value->GetPosition();

    return { pos.x, pos.y + visualHeight * 1.5f };
}

Player::Player(Child *parent, std::string color, float x, float y) : Child(parent), color(std::move(color)) {
    client = find<Client>();
    camera = find<Resources>()->camera;

    spawnX = x;
    spawnY = y;

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
