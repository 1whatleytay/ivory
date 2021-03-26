#include <ores/player.h>

#include <ores/client.h>
#include <ores/asset-loader.h>

#include <fmt/printf.h>

void Player::setAnimation(const TagInfo &animation) {
    currentAnimation = &animation;
    currentFrame = 0;
}

void Player::update(float time) {
    float x = glfwGetKey(engine.window, GLFW_KEY_D) - glfwGetKey(engine.window, GLFW_KEY_A);
    float y = glfwGetKey(engine.window, GLFW_KEY_W) - glfwGetKey(engine.window, GLFW_KEY_S);

    constexpr float playerSpeed = 4;

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

    visual->set(pos.x, pos.y + visualHeight / 4, visualWidth, visualHeight, *frames[i], -0.13);

    size_t a = 0;

    constexpr float offset = 0.03;

    for (int64_t ox = -1; ox <= 1; ox++) {
        for (int64_t oy = -1; oy <= 1; oy++) {
            if (ox == 0 && oy == 0)
                continue;

            outlines[a++]->set(pos.x + ox * offset, pos.y + visualHeight / 4 + oy * offset,
                visualWidth, visualHeight, *frames[i], -0.125);
        }
    }

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

void Player::draw() {
    glUseProgram(engine.outlineProgram);
    for (auto &o : outlines)
        o->draw();
    glUseProgram(engine.program);
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
            setAnimation(walkRight);
        else if (x < 0)
            setAnimation(walkLeft);
        else if (y > 0)
            setAnimation(walkUp);
        else if (y < 0)
            setAnimation(walkDown);
        else if (left)
            setAnimation(idleLeft);
        else
            setAnimation(idleRight);
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

Player::Player(Child *parent) : Child(parent) {
    client = find<Client>();

    float x = client ? client->hello.playerX : 0.0f;
    float y = client ? client->hello.playerY : 0.5f;

    AssetLoader loader((engine.assets / "images/players/nate.json").string(), engine.assets.string());

    auto size = loader.size();

    auto tex = assemble<parts::Texture>(loader.image.width, loader.image.height, loader.image.data.get());
    frames.resize(loader.frames.size());
    for (size_t a = 0; a < loader.frames.size(); a++) {
        const FrameInfo &info = loader.frames[a];

        frames[a] = tex->grab(info.x, info.y, info.width, info.height);
        assert(frames[a]);
    }

    idleLeft = loader.tags["idle left"];
    idleRight = loader.tags["idle right"];

    walkLeft = loader.tags["walk left"];
    walkRight = loader.tags["walk right"];
    walkUp = loader.tags["walk up"];
    walkDown = loader.tags["walk down"];

    setAnimation(idleRight);

    constexpr float targetHeight = 0.75;

    visualHeight = targetHeight;
    visualWidth = targetHeight / size.second * size.first;

    visual = make<parts::BoxVisual>();
    outlines.resize(8);
    for (auto &o : outlines)
        o = create<parts::BoxVisual>();
    body = make<parts::BoxBody>(x, y, visualWidth, visualHeight / 2, 1);
}
