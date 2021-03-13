#pragma once

#include <glad/glad.h>

#include <functional>

struct Handle {
    using Deleter = std::function<void(GLuint)>;
    using PointerModifier = std::function<void(GLsizei, GLuint *)>;

    GLuint value = 0;
    Deleter deleter;

    void reset(GLuint value, Deleter deleter);
    void reset(const PointerModifier &onCreate, const PointerModifier &onDelete);

    void reset(GLuint v);
    void reset(const PointerModifier &onCreate);

    operator GLuint() const;

    Handle &operator =(Handle &&o) noexcept;

    Handle(const Handle &o) = delete;
    Handle(Handle &&o) noexcept;

    Handle() = default;
    Handle(GLuint value, Deleter deleter);
    Handle(const PointerModifier &onCreate, const PointerModifier &onDelete);

    ~Handle();
};
