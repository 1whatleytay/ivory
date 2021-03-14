#pragma once

#include <engine/handle.h>
#include <engine/vertex.h>

#include <GLFW/glfw3.h>
#include <box2d/box2d.h>

#include <array>
#include <string>
#include <vector>
#include <unordered_map>

struct Child;

struct Engine {
    static std::string assets;

    GLFWwindow *window = nullptr;
    b2World world;

    Handle program;

    GLint offsetUniform;
    GLint scaleUniform;

    float offsetX = 0, offsetY = 0;

    Child *app = nullptr;

    Color sky;

    float zoom = 80;

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

    explicit Engine(GLFWwindow *window);
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

    Holder<T> &operator =(Holder<T> &&o) noexcept {
        reset(o.parent, o.value);

        o.value = nullptr;
        o.parent = nullptr;
    }

    Holder(Holder &&o) noexcept {
        reset(o.parent, o.value);

        o.value = nullptr;
        o.parent = nullptr;
    }

    Holder(const Holder &holder) = delete;
    Holder(Child *parent, T *value) : parent(parent), value(value) { }
    ~Holder() { reset(nullptr, nullptr); }
};

struct Child {
    Engine &engine;
    Child *parent = nullptr;

    template <typename E>
    E &as() { return dynamic_cast<E &>(*this); }

    template <typename E>
    E &root() { return dynamic_cast<E &>(*engine.app); }

    std::vector<std::unique_ptr<Child>> children;
    std::unordered_map<size_t, std::unique_ptr<Resource>> resources;

    virtual void draw();
    virtual void update(float time);
    virtual void click(int button, int action);
    virtual void keyboard(int key, int action);

    void engineDraw();
    void engineUpdate(float time);
    void engineClick(int button, int action);
    void engineKeyboard(int key, int action);

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

        Child *temp = i->release();
        children.erase(i);

        ptr = nullptr;

        return std::unique_ptr<T>(dynamic_cast<T *>(temp));
    }

    Resource *find(size_t hash);

    template <typename T>
    T *find() { return dynamic_cast<T *>(find(typeid(T).hash_code())); }

    template <typename T, typename ...Args>
    std::unique_ptr<T> create(const Args &... args) {
        return std::make_unique<T>(this, args...);
    }

    template <typename T, typename ...Args>
    T *supply(const Args &... args) {
        auto c = std::make_unique<T>(this, args...);

        T *x = c.get();
        resources[typeid(T).hash_code()] = std::move(c);

        return x;
    }

    template <typename T, typename ...Args>
    T *get(const Args &... args) {
        auto *z = find<T>();

        return z ? z : supply<T>(args...);
    }

    template <typename T>
    std::unique_ptr<T> destroy(T * &ptr) {
        auto match = [ptr](const auto &p) { return p.second.get() == ptr; };
        auto i = std::find_if(resources.begin(), resources.end(), match);

        if (i == resources.end())
            return nullptr;

        Resource *temp = i->second.release();
        resources.erase(i);

        ptr = nullptr;

        return std::unique_ptr<T>(dynamic_cast<T *>(temp));
    }

    explicit Child(Child *parent);
    explicit Child(Engine &engine);
    virtual ~Child() = default;
};

template <typename T>
void Holder<T>::reset(Child *p, T *v) {
    if (parent && value)
        parent->drop(value);

    value = v;
    parent = p;
}
