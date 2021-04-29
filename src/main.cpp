#include <content/game.h>
#include <content/options.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <boxer/boxer.h>

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

    GLFWwindow *window = glfwCreateWindow(800, 600, "Avastus", nullptr, nullptr);
    if (!window)
        throw std::exception();

    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (!gladLoadGL())
        throw std::exception();

    fs::path assets;

    if (!options.assetsPath.empty()) {
        assets = options.assetsPath;
    } else if (count > 0) {
        assets = fs::path(args[0]).parent_path() / "assets";
    } else {
        assets = "assets";
    }

    try {
        std::make_unique<Engine>(window, assets)->run<Game>(options);
    } catch (const std::runtime_error &e) {
        boxer::show(e.what(), "Error", boxer::Style::Error);
    } catch (const std::exception &e) {
        boxer::show("An unknown error occurred.", "Error", boxer::Style::Error);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

	return 0;
}