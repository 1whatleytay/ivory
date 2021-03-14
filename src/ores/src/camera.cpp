#include <ores/camera.h>

void Camera::update(float time) {
    constexpr float speed = 20;

    if (glfwGetKey(engine.window, GLFW_KEY_LEFT) == GLFW_PRESS)
        engine.offsetX += speed * time;
    if (glfwGetKey(engine.window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        engine.offsetX -= speed * time;
    if (glfwGetKey(engine.window, GLFW_KEY_DOWN) == GLFW_PRESS)
        engine.offsetY += speed * time;
    if (glfwGetKey(engine.window, GLFW_KEY_UP) == GLFW_PRESS)
        engine.offsetY -= speed * time;

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
        engine.zoom *= 1.01;

        scale();
    }
}

Camera::Camera(Child *parent) : Child(parent) { }
