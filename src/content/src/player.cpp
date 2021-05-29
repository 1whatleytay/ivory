#include <content/player.h>

#include <content/camera.h>
#include <content/client.h>
#include <content/capture.h>
#include <content/resources.h>

#include <fmt/printf.h>

void Player::revertAnimation() {
    int x = glfwGetKey(engine.window, GLFW_KEY_D) - glfwGetKey(engine.window, GLFW_KEY_A);
    int y = glfwGetKey(engine.window, GLFW_KEY_W) - glfwGetKey(engine.window, GLFW_KEY_S);

    if (x == 0 && y == 0)
        pickAnimation(idle);
    else
        pickAnimation(walk, selectLinearDirection(x, y));
}

void Player::doMovement() {
    float x = glfwGetKey(engine.window, GLFW_KEY_D) - glfwGetKey(engine.window, GLFW_KEY_A);
    float y = glfwGetKey(engine.window, GLFW_KEY_W) - glfwGetKey(engine.window, GLFW_KEY_S);

    if (x != 0 || y != 0)
        direction = selectLinearDirection(x, y);

    if (dieTimeout > 0 || isLocked)
        return;

    float playerSpeed = 5;

    if (holding) {
        if (holding->color == color)
            playerSpeed = 3;
        else
            playerSpeed = 3.5;
    }

    b2Vec2 velo(x, y);
    velo.Normalize();
    velo *= playerSpeed;

    body->value->SetLinearVelocity(velo);
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

            currentAnimation->name
        });
    }
}

void Player::update(float time) {
    doMovement();
    doNetwork(time);

    PlayerBase::update(time);

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

void Player::click(int button, int action) {
    if (action == GLFW_PRESS) {
        if (button == GLFW_MOUSE_BUTTON_1 && !isPunching) {
            auto pos = body->value->GetPosition();

            direction = selectDirection(Position { pos.x, pos.y }, engine.cursor());

            auto doIdle = [this]() {
                isLocked = false;
                revertAnimation();
            };

            auto doEnd = [this, doIdle]() {
                isPunching = false;
                pickAnimation(punch.end, doIdle);
            };

            auto doRecoil = [this, doEnd]() {
                pickAnimation(punch.recoil, doEnd);
            };

            auto doWindup = [this, doRecoil]() {
                pickAnimation(punch.windup, doRecoil);
            };

            isLocked = true;
            isPunching = true;
            pickAnimation(punch.start, doWindup);
        }
    }
}

void Player::keyboard(int key, int action) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_U) {
            netUpdateIndex++;
        }

//        if (key == GLFW_KEY_W)
//            direction = Direction::Up;
//        if (key == GLFW_KEY_S)
//            direction = Direction::Down;
//        if (key == GLFW_KEY_A)
//            direction = Direction::Left;
//        if (key == GLFW_KEY_D)
//            direction = Direction::Right;

        if (key == GLFW_KEY_K && !isPunching) {
            auto doIdle = [this]() {
                isLocked = false;
                revertAnimation();
            };

            auto doEnd = [this, doIdle]() {
                isPunching = false;
                pickAnimation(punch.end, doIdle);
            };

            auto doRecoil = [this, doEnd]() {
                pickAnimation(punch.recoil, doEnd);
            };

            auto doWindup = [this, doRecoil]() {
                pickAnimation(punch.windup, doRecoil);
            };

            isLocked = true;
            isPunching = true;
            pickAnimation(punch.start, doWindup);
        }

        // Interact
        if (key == GLFW_KEY_F) {
            if (holding) {
                Flag *flag = holding;

                holding->pickUp(nullptr);

                auto position = flag->body->GetPosition();

                if (client) {
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
                                    pos.x, pos.y
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

    if (action != GLFW_REPEAT && isMovementKey() && !isLocked) {
        revertAnimation();
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

Player::Player(Child *parent, std::string color, float x, float y) : PlayerBase(parent, x, y), color(std::move(color)) {
    client = find<Client>();
    camera = find<Resources>()->camera;

    spawnX = x;
    spawnY = y;
}
