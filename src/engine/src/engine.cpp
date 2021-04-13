#include <engine/engine.h>

#include <glad/glad.h>

#include <chrono>

#include <fmt/printf.h>

#include <array>
#include <fstream>
#include <sstream>

namespace {
    Handle loadShaderProgram(const std::string &vertPath, const std::string &fragPath) {
        auto getSource = [](const std::string &path) -> std::string {
            std::ifstream stream(path);
            if (!stream.is_open())
                throw std::exception();

            std::stringstream buffer;
            buffer << stream.rdbuf();

            return buffer.str();
        };

        std::string vertSource = getSource(vertPath);
        std::string fragSource = getSource(fragPath);

        const char *vertPtr = vertSource.c_str();
        const char *fragPtr = fragSource.c_str();

        Handle vertShader(glCreateShader(GL_VERTEX_SHADER), glDeleteShader);
        Handle fragShader(glCreateShader(GL_FRAGMENT_SHADER), glDeleteShader);

        glShaderSource(vertShader, 1, &vertPtr, nullptr);
        glShaderSource(fragShader, 1, &fragPtr, nullptr);

        auto verify = [](GLuint s) {
            GLint status;
            glGetShaderiv(s, GL_COMPILE_STATUS, &status);

            if (!status) {
                std::array<char, 1000> buffer = { };

                int32_t length;
                glGetShaderInfoLog(s, buffer.size(), &length, buffer.data());

                fmt::print("Compile Error: {}\n", std::string(buffer.data(), length));

                throw std::exception();
            }
        };

        glCompileShader(vertShader);
        glCompileShader(fragShader);

        verify(vertShader);
        verify(fragShader);

        Handle program(glCreateProgram(), glDeleteProgram);

        glAttachShader(program, vertShader);
        glAttachShader(program, fragShader);
        glLinkProgram(program);

        GLint status;
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if (!status) {
            std::array<char, 1000> buffer = { };

            int32_t length;
            glGetProgramInfoLog(program, buffer.size(), &length, buffer.data());

            fmt::print("Link Error: {}\n", std::string(buffer.data(), length));

            throw std::exception();
        }

        return program;
    }
}

Resource::Resource(Child *component) : component(component) { }

Child::Child(Engine &engine) : engine(engine) {
    engine.drawList.push_back(this);
}

Child::Child(Child *parent) : parent(parent), engine(parent->engine) {
    engine.drawList.push_back(this);
}

Child::~Child() {
    auto &list = engine.drawList;
    auto iterator = std::find(list.begin(), list.end(), this);

    if (iterator != list.end())
        list.erase(iterator);
}

void Child::layerAfter(Child *e) {
    auto &list = engine.drawList;

    using it = decltype(engine.drawList)::iterator;

    auto i = list.end();
    auto t = list.end();

    auto isValid = [&i, &t, &list]() {
        return i != list.end() && t != list.end();
    };

    for (auto b = list.begin(); b != list.end() && !isValid(); b++) {
        if (*b == this)
            t = b;
        else if (*b == e)
            i = b;
    }

    if (!isValid())
        return;

    if (t > i) {
        std::rotate(i + 1, t, t + 1);
    } else {
        std::rotate(
            list.rbegin() + std::distance(i, list.end()) - 1,
            list.rbegin() + std::distance(t, list.end()) - 1,
            list.rbegin() + std::distance(t, list.end())
        );
    }
}

void Child::draw() { }
void Child::update(float time) { }
void Child::click(int button, int action) { }
void Child::keyboard(int key, int action) { }

Resource *Child::find(std::type_index i) {
    auto x = supplies.find(i);

    return x == supplies.end() ? (parent ? parent->find(i) : nullptr) : (x->second.get());
}

Bounds Engine::bounds() {
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    float screenWidth = static_cast<float>(width) / zoom;
    float screenHeight = static_cast<float>(height) / zoom;

    return Bounds {
        -screenWidth / 2 - offsetX, screenHeight / 2 - offsetY,
        screenWidth, screenHeight
    };
}

void Engine::key(int key, int action) const {
    app->call(&Child::keyboard, key, action);
}

void Engine::click(int button, int action) const {
    app->call(&Child::click, button, action);
}

void Engine::scale(int width, int height) const {
    glUniform2f(scaleUniform, static_cast<float>(width) / zoom, static_cast<float>(height) / zoom);
}

namespace {
    int64_t now() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
}

void Engine::execute() {
    int64_t last = now();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(sky.red, sky.green, sky.blue, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        auto current = now();

        app->call(&Child::update, static_cast<float>(current - last) / 1000.0f);

        glUseProgram(program);
        glUniform2f(offsetUniform, offsetX, offsetY);

        world.Step(1 / 60.0f, 6, 2);

        for (Child *d : drawList)
            d->draw();

        last = current;

        glfwSwapBuffers(window);
    }
}

Engine::Engine(GLFWwindow *window, fs::path assets)
    : assets(std::move(assets)), window(window), world(b2Vec2(0.0f, 0.0f)), sky(0x0099DB) {

    glfwSetWindowUserPointer(window, this);

    glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        reinterpret_cast<Engine *>(glfwGetWindowUserPointer(window))->key(key, action);
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow *window, int button, int action, int mods) {
        reinterpret_cast<Engine *>(glfwGetWindowUserPointer(window))->click(button, action);
    });

    glfwSetWindowSizeCallback(window, [](GLFWwindow *window, int width, int height) {
        reinterpret_cast<Engine *>(glfwGetWindowUserPointer(window))->scale(width, height);
    });

    program = loadShaderProgram(
        (this->assets / "shaders/vert.glsl").string(),
        (this->assets / "shaders/frag.glsl").string());

    glUseProgram(program);
    scaleUniform = glGetUniformLocation(program, "scale");
    offsetUniform = glGetUniformLocation(program, "offset");
    assert(offsetUniform >= 0 && scaleUniform >= 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    scale(width, height);
}