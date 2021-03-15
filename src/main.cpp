#include <ores/game.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fmt/printf.h>

int main(int count, const char **args) {
    fmt::print("Starting.\n");

    if (!glfwInit())
        throw std::exception();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "Game", nullptr, nullptr);
    if (!window)
        throw std::exception();

    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (!gladLoadGL())
        throw std::exception();

    auto clean = [](std::string input) {
        int64_t x;
        while ((x = input.find('\\')) != std::string::npos) {
            input[x] = '/';
        }

        return input;
    };

    if (count > 1) {
        Engine::assets = clean(args[1]);
    } else if (count == 1) {
        std::string path = clean(args[0]);

        auto x = path.rfind('/');
        if (x != std::string::npos)
            path = path.substr(0, x);

        Engine::assets = path + "/assets";
    } else {
        Engine::assets = "assets";
    }

    if (Engine::assets[Engine::assets.size() - 1] != '/')
        Engine::assets += '/';

    std::make_unique<Engine>(window)->run<Game>();

    glfwDestroyWindow(window);
    glfwTerminate();

	return 0;
}