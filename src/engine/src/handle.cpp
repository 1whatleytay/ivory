#include <engine/handle.h>

void Handle::reset(GLuint v, Deleter d) {
    if (value)
        deleter(value);

    value = v;
    deleter = std::move(d);
}

void Handle::reset(GLuint v) { reset(v, deleter); }

void Handle::reset(const PointerModifier &onCreate, const PointerModifier &onDelete) {
    GLuint v;
    onCreate(1, &v);

    reset(v, [onDelete](GLuint t) { onDelete(1, &t); });
}

void Handle::reset(const PointerModifier &onCreate) {
    GLuint v;
    onCreate(1, &v);

    reset(v);
}

Handle::operator GLuint() const {
    return value;
}

Handle::Handle(Handle &&o) noexcept {
    value = o.value;
    o.value = 0;
}

Handle::Handle(GLuint value, Deleter deleter) { reset(value, std::move(deleter)); }
Handle::Handle(const PointerModifier &onCreate, const PointerModifier &onDelete) { reset(onCreate, onDelete); }

Handle::~Handle() { reset(0); }
