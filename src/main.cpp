#include <ores/game.h>
#include <ores/options.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fmt/printf.h>

int main(int count, const char **args) {
    fmt::print("Starting.\n");

    Options options(count, args);

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

    std::string assets;

    if (!options.assetsPath.empty()) {
        assets = options.assetsPath;
    } else if (count > 0) {
        std::string path = clean(args[0]);

        auto x = path.rfind('/');
        if (x != std::string::npos)
            path = path.substr(0, x);

        assets = path + "/assets";
    } else {
        assets = "assets";
    }

    if (assets[assets.size() - 1] != '/')
        assets += '/';

    std::make_unique<Engine>(window, assets)->run<Game>(options);

    glfwDestroyWindow(window);
    glfwTerminate();

	return 0;
}