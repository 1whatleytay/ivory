#pragma once

#include <engine/handle.h>
#include <engine/engine.h>

#include <any>
#include <optional>

namespace parts {
    struct Buffer;

    struct BufferRange {
        Buffer *parent = nullptr;

        size_t begin = 0;
        size_t size = 0;

        void draw() const;
        BufferRange &write(void *data);

        BufferRange(Buffer *parent, size_t begin, size_t size);
    };

    struct Buffer : public Resource {
        size_t vertices = 0;

        std::vector<bool> taken;

        Handle buffer;
        Handle vao;

        std::vector<std::pair<std::unique_ptr<Buffer>, std::unique_ptr<BufferRange>>> children;

        void free(BufferRange * &ptr);

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

        void free(TextureRange * &ptr);

        void bind() const;

        TextureRange *grab(size_t w, size_t h, void *data = nullptr);
        TextureRange *grab(size_t x, size_t y, size_t w, size_t h);
        std::vector<TextureRange *> grabTileset(size_t w, size_t h);

        Texture(Child *component, GLenum filter = GL_NEAREST);
        Texture(Child *component, size_t width, size_t height, void *data = nullptr, GLenum filter = GL_NEAREST);
    };

    struct BoxBody : public Child {
        b2Body *value = nullptr;

        float weight = 0;
        float width = 0, height = 0;

//        std::any user;

        [[nodiscard]] bool isGround() const;

        BoxBody(Child *parent, float x, float y, float width, float height, float weight = 0);
        ~BoxBody() override;
    };

    struct BoxVisual : public Child {
        BufferRange *range = nullptr;
        const TextureRange *tex = nullptr;

        void set(float x, float y, float width, float height, const TextureRange &texture,
            bool flipX = false, bool flipY = false);
        void set(const BoxBody &body, const TextureRange &texture,
            bool flipX = false, bool flipY = false);

        void draw() override;

        explicit BoxVisual(Child *parent);
    };

    struct Box : public Child {
        BoxBody *body;
        BoxVisual *visual;

        TextureRange *texture;

        bool alive = true;

        bool flipX = false, flipY = false;

        void update(float time) override;

        Box(Child *parent, float x, float y, float width, float height, Color color, float weight = 0);
        Box(Child *parent, float x, float y, float width, float height, TextureRange *texture, float weight = 0);
    };

    template <typename T>
    using WorldPtr = std::unique_ptr<T, std::function<void(T *)>>;

    struct BodyPtr : WorldPtr<b2Body> {
        BodyPtr();
    };

    struct JointPtr : WorldPtr<b2Joint> {
        b2World &w;

        JointPtr(b2World &w);
    };

    namespace shapes {
        std::array<Vertex, 6> square(float x, float y, float width, float height,
            const TextureRange *texture, bool flipX = false, bool flipY = false);
    }
}
