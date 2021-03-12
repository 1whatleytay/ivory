#include <ores/camera.h>

void Camera::update(float time) {
    constexpr float speed = 5;

    if (glfwGetKey(engine.window, GLFW_KEY_LEFT) == GLFW_PRESS)
        engine.offsetX += speed * time;
    if (glfwGetKey(engine.window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        engine.offsetX -= speed * time;
    if (glfwGetKey(engine.window, GLFW_KEY_DOWN) == GLFW_PRESS)
        engine.offsetY += speed * time;
    if (glfwGetKey(engine.window, GLFW_KEY_UP) == GLFW_PRESS)
        engine.offsetY -= speed * time;
}

Camera::Camera(Child *parent) : Child(parent) { }
