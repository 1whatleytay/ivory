#pragma once

#include <engine/handle.h>
#include <engine/engine.h>

#include <optional>

namespace parts {
    struct Buffer;

    struct BufferRange {
        Buffer *parent = nullptr;

        size_t begin = 0, size = 0;

        void draw() const;
        BufferRange &write(void *data);

        BufferRange(Buffer *parent, size_t begin, size_t size);
    };

    struct Buffer : public Resource {
        size_t vertices = 0;

        size_t allocated = 0;

        Handle buffer;
        Handle vao;

        std::vector<std::pair<std::unique_ptr<Buffer>, std::unique_ptr<BufferRange>>> children;

        void bind() const;

        BufferRange *grab(size_t count, void *data = nullptr);

        Buffer(Child *component, size_t vertices);
    };

    struct Texture;

    struct TextureRange {
        Texture *parent;

        size_t x, y;
        size_t w, h;

        TextureRange &write(void *data);

        TextureRange(Texture *parent, size_t x, size_t y, size_t w, size_t h);
    };

    struct Texture : public Resource {
        size_t width, height;

        std::vector<bool> taken;

        std::vector<std::pair<std::unique_ptr<Texture>, std::unique_ptr<TextureRange>>> children;

        Handle texture;
        Handle sampler;

        void bind() const;

        TextureRange *grab(size_t w, size_t h, void *data = nullptr);
        std::vector<TextureRange *> grabTileset(size_t w, size_t h);

        Texture(Child *component);
        Texture(Child *component, size_t width, size_t height, void *data = nullptr);
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
        BufferRange *range = nullptr;
        const TextureRange *tex = nullptr;

        void set(float x, float y, float width, float height, const TextureRange &texture, float depth = 0);
        void set(const BoxBody &body, const TextureRange &texture, float depth = 0);

        void draw() override;

        explicit BoxVisual(Child *parent);
    };

    struct Box : public Child {
        BoxBody *body;
        BoxVisual *visual;

        TextureRange *texture;
        float depth = 0;

        bool alive = true;

        void update(float time) override;

        Box(Child *parent, float x, float y, float width, float height, Color color, float weight = 0);
        Box(Child *parent, float x, float y, float width, float height, TextureRange *texture, float weight = 0);
    };

    namespace shapes {
        std::array<Vertex, 6> square(float x, float y, float width, float height,
            const TextureRange *texture, float depth = 0);
    }
}
