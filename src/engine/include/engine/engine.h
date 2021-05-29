#pragma once

#include <engine/handle.h>
#include <engine/vertex.h>

#include <GLFW/glfw3.h>
#include <box2d/box2d.h>

#include <array>
#include <string>
#include <vector>
#include <filesystem>
#include <typeindex>
#include <unordered_map>

namespace fs = std::filesystem;

struct Child;
struct Bounds;

struct Position {
    float x, y;

    bool within(const Bounds &bounds) const;
};

struct Bounds {
    float x, y;
    float width, height;
};

struct Engine {
    fs::path assets;

    GLFWwindow *window = nullptr;
    b2World world;

    float physicsUpdateTime = 0;
    static constexpr float physicsUpdateInterval = 1 / 60.0f;

    Handle program;

    GLint offsetUniform;
    GLint scaleUniform;

    Child *app = nullptr;

    Color sky;

    std::vector<Child *> drawList;

    float zoom = 80;
    float offsetX = 0, offsetY = 0;

    Bounds bounds() const;
    Position cursor() const;

    void key(int key, int action) const;
    void scale(int width, int height) const;
    void click(int button, int action) const;

    void execute();

    template <typename T, typename ...Args>
    void run(const Args &... args) {
        T t(*this, args...);

        app = &t;
        execute();
        app = nullptr;
    }

    explicit Engine(GLFWwindow *window, fs::path assets = "assets");
};

struct Resource {
    Child *component = nullptr;

    explicit Resource(Child *component);
    virtual ~Resource() = default;
};

template <typename T>
struct Holder {
    T *value = nullptr;
    Child *parent = nullptr;

    T *operator ->() { return value; }

    void reset(Child *parent, T *value);

    void reset() {
        reset(nullptr, nullptr);
    }

    Holder<T> &operator =(Holder<T> &&o) noexcept {
        reset(o.parent, o.value);

        o.value = nullptr;
        o.parent = nullptr;

        return *this;
    }

    Holder(Holder &&o) noexcept {
        reset(o.parent, o.value);

        o.value = nullptr;
        o.parent = nullptr;
    }

    Holder() = default;
    Holder(const Holder &holder) = delete;
    Holder(Child *parent, T *value) : parent(parent), value(value) { }
    ~Holder() { reset(nullptr, nullptr); }
};

struct Child {
    Engine &engine;
    Child *parent = nullptr;

    std::vector<std::unique_ptr<Child>> children;
    std::vector<std::unique_ptr<Resource>> resources;
    std::unordered_map<std::type_index, std::unique_ptr<Resource>> supplies;

    void layerTop();
    void layerAfter(Child *e);

    virtual void draw();
    virtual void update(float time);
    virtual void click(int button, int action);
    virtual void keyboard(int key, int action);

    template <typename E>
    E &as() { return dynamic_cast<E &>(*this); }

    template <typename E>
    E &root() { return dynamic_cast<E &>(*engine.app); }

    template <typename ... Args1, typename ... Args2>
    void call(void (Child:: * f)(Args1 ...), Args2 &&... args) {
        (this->*f)(args...);

        for (const auto &e : children)
            e->call(f, args...);
    }

    template <typename T, typename ...Args>
    std::unique_ptr<T> create(const Args &... args) {
        return std::make_unique<T>(this, args...);
    }

    template <typename T, typename ...Args>
    T *make(const Args &... args) {
        auto c = std::make_unique<T>(this, args...);

        T *x = c.get();

        children.push_back(std::move(c));

        return x;
    }

    template <typename T, typename ...Args>
    Holder<T> hold(const Args &... args) {
        return Holder<T>(this, make<T>(args...));
    }

    template <typename T>
    std::unique_ptr<T> drop(T * &ptr) {
        auto match = [ptr](const auto &p) { return p.get() == ptr; };
        auto i = std::find_if(children.begin(), children.end(), match);

        if (i == children.end())
            return nullptr;

        auto *temp = dynamic_cast<T *>(i->release());
        children.erase(i);

        ptr = nullptr;

        return std::unique_ptr<T>(temp);
    }

    template <typename T>
    std::unique_ptr<T> dropResource(T * &ptr) {
        auto match = [ptr](const auto &p) { return p.get() == ptr; };
        auto i = std::find_if(resources.begin(), resources.end(), match);

        if (i == resources.end())
            return nullptr;

        auto *temp = dynamic_cast<T *>(i->release());
        resources.erase(i);

        ptr = nullptr;

        return std::unique_ptr<T>(temp);
    }

    Resource *find(std::type_index i);

    template <typename T, typename ...Args>
    T *assemble(const Args &... args) {
        auto c = create<T>(args...);

        T *x = c.get();
        resources.push_back(std::move(c));

        return x;
    }

    template <typename T>
    T *find() { return dynamic_cast<T *>(find(std::type_index(typeid(T)))); }

    template <typename T, typename ...Args>
    T *supply(const Args &... args) {
        auto c = create<T>(args...);

        T *x = c.get();
        std::type_index i(typeid(T));

        auto &e = supplies[i];

        if (e)
            resources.push_back(std::move(e));

        e = std::move(c);

        return x;
    }

    template <typename T, typename ...Args>
    T *get(const Args &... args) {
        auto *z = find<T>();

        return z ? z : supply<T>(args...);
    }

    template <typename T>
    std::unique_ptr<T> destroy() {
        auto i = std::find(supplies.begin(), supplies.end(), std::type_index(typeid(T)));

        if (i == supplies.end())
            return nullptr;

        Resource *temp = i->second.release();
        supplies.erase(i);

        return std::unique_ptr<T>(dynamic_cast<T *>(temp));
    }

    explicit Child(Child *parent);
    explicit Child(Engine &engine);
    virtual ~Child();
};

template <typename T>
void Holder<T>::reset(Child *p, T *v) {
    if (parent && value)
        parent->drop(value);

    value = v;
    parent = p;
}
