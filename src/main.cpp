#include <ores/game.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fmt/printf.h>

int main() {
    fmt::print("Starting.\n");

    if (!glfwInit())
        throw std::exception();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "Pizza", nullptr, nullptr);
    if (!window)
        throw std::exception();

    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (!gladLoadGL())
        throw std::exception();

    Engine(window).run<Game>();

    glfwDestroyWindow(window);
    glfwTerminate();

	return 0;
}