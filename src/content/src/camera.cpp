#include <content/camera.h>

#include <content/font.h>
#include <content/player.h>
#include <content/resources.h>

#include <fmt/format.h>

void Camera::update(float time) {
    constexpr float speed = 20;

    if (usePlayer && resources->player) {
        auto pos = resources->player->body->value->GetPosition();

        engine.offsetX = -pos.x;
        engine.offsetY = -pos.y;
    } else {
        if (glfwGetKey(engine.window, GLFW_KEY_LEFT) == GLFW_PRESS)
            engine.offsetX += speed * time;
        if (glfwGetKey(engine.window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            engine.offsetX -= speed * time;
        if (glfwGetKey(engine.window, GLFW_KEY_DOWN) == GLFW_PRESS)
            engine.offsetY += speed * time;
        if (glfwGetKey(engine.window, GLFW_KEY_UP) == GLFW_PRESS)
            engine.offsetY -= speed * time;
    }

    auto scale = [this]() {
        int w, h;
        glfwGetWindowSize(engine.window, &w, &h);

        engine.scale(w, h);
    };

    if (glfwGetKey(engine.window, GLFW_KEY_O) == GLFW_PRESS) {
        engine.zoom *= 0.99;

        scale();
    }

    if (glfwGetKey(engine.window, GLFW_KEY_P) == GLFW_PRESS) {
        engine.zoom /= 0.99;

        scale();
    }

    redScoreText->set(fmt::format("Red Score: {}", redScore), -engine.offsetX - 4.6, -engine.offsetY - 3.4);
    blueScoreText->set(fmt::format("Blue Score: {}", blueScore), -engine.offsetX - 4.6, -engine.offsetY - 3);
}

void Camera::keyboard(int key, int action) {
    if (action == GLFW_PRESS && key == GLFW_KEY_M) {
        usePlayer = !usePlayer;
    }
}

Camera::Camera(Child *parent) : Child(parent), resources(find<Resources>()) {
    redScoreText = make<FontText>();
    blueScoreText = make<FontText>();
}
