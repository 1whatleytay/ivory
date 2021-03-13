#pragma once

#include <engine/handle.h>
#include <engine/engine.h>

#include <optional>

namespace parts {
    struct Buffer;

    struct Range {
        Buffer *parent = nullptr;
        std::unique_ptr<Buffer> owned;

        [[nodiscard]] Buffer *get() const;

        size_t begin = 0, size = 0;

        void draw() const;
        void write(void *data) const;

        Range(Buffer *buffer, size_t begin, size_t size);
        Range(std::unique_ptr<Buffer> owned, size_t begin, size_t size);
    };

    struct Buffer : public Resource {
        size_t vertices = 0;

        size_t allocated = 0;

        Handle buffer;
        Handle vao;

        void bind() const;

        Range grab(size_t count);

        explicit Buffer(Child *component, size_t vertices);
    };

    struct BoxBody : public Child {
        b2Body *value = nullptr;

        float weight = 0;
        float width = 0, height = 0;

        [[nodiscard]] bool isGround() const;

        void setVelocity(std::optional<float> x, std::optional<float> y);
        void capVelocity(std::optional<float> x, std::optional<float> y);

        BoxBody(Child *parent, float x, float y, float width, float height, float weight = 0);
        ~BoxBody() override;
    };

    struct BoxVisual : public Child {
        Range range;

        void set(float x, float y, float width, float height, const Color &color) const;
        void set(const BoxBody &body, const Color &color) const;

        void draw() override;

        explicit BoxVisual(Child *parent);
    };

    struct Box : public Child {
        BoxBody *body;
        BoxVisual *visual;

        Color color;

        void update(float time) override;

        explicit Box(Child *parent, float x, float y, float width, float height, Color color, float weight = 0);
    };
}
