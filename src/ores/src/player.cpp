#include <ores/player.h>

#include <ores/client.h>
#include <ores/asset-loader.h>

#include <fmt/printf.h>

void Player::setAnimation(const TagInfo &animation) {
    currentAnimation = &animation;
    currentFrame = 0;
}

void Player::update(float time) {
    if (glfwGetKey(engine.window, GLFW_KEY_A) == GLFW_PRESS)
        box->body->setVelocity(-5, std::nullopt);
    if (glfwGetKey(engine.window, GLFW_KEY_D) == GLFW_PRESS)
        box->body->setVelocity(+5, std::nullopt);
    if (glfwGetKey(engine.window, GLFW_KEY_W) == GLFW_PRESS)
        box->body->setVelocity(std::nullopt, +5);
    if (glfwGetKey(engine.window, GLFW_KEY_S) == GLFW_PRESS)
        box->body->setVelocity(std::nullopt, -5);


    constexpr float frameDuration = 0.1;
    frameUpdateTime += time;

    while (frameUpdateTime > frameDuration) {
        currentFrame++;
        frameUpdateTime -= frameDuration;
    }

    size_t i = currentFrame % (currentAnimation->end - currentAnimation->start + 1) + currentAnimation->start;

    box->texture = frames[i];

    if (client) {
        float timeToGo = netUpdates[netUpdateIndex % netUpdates.size()];

        netUpdateTime += time;
        if (netUpdateTime > timeToGo) {
            if (timeToGo == 0) {
                netUpdateTime = 0;
            } else {
                netUpdateTime = std::fmod(netUpdateTime, timeToGo);
            }

            auto pos = box->body->value->GetPosition();

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

            box->body->value->SetTransform(b2Vec2(x, y), 0);
            box->body->value->SetAwake(true);
        }

        if (key == GLFW_KEY_U) {
            netUpdateIndex++;
        }
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

        box->body->value->SetTransform(b2Vec2(cX, cY), 0);
    }
}

Player::Player(Child *parent) : Child(parent) {
    client = find<Client>();

    float x = client ? client->hello.playerX : 0;
    float y = client ? client->hello.playerY : 0.5;

    AssetLoader loader(engine.assets / "images/players/nate.json", engine.assets);

    auto size = loader.size();

    auto tex = assemble<parts::Texture>(loader.image.width, loader.image.height, loader.image.data.get());
    frames = tex->grabTileset(size.first, size.second);

    idle = loader.tags["walk right"];

    setAnimation(idle);

    box = make<parts::Box>(x, y, 0.39, 0.75, Color(0xFF0000u), 1);
    box->depth = -0.07;
}
