#include <engine/parts.h>

namespace parts {
    void BufferRange::draw() const {
        get()->bind();

        glDrawArrays(GL_TRIANGLES, begin, size);
    }

    BufferRange &BufferRange::write(void *data) {
        get()->bind();

        glBufferSubData(GL_ARRAY_BUFFER, begin * sizeof(Vertex), size * sizeof(Vertex), data);

        return *this;
    }

    Buffer *BufferRange::get() const {
        return owned ? owned.get() : parent;
    }

    BufferRange::BufferRange(Buffer *parent, size_t begin, size_t size)
        : parent(parent), begin(begin), size(size) { }
    BufferRange::BufferRange(std::unique_ptr<Buffer> owned, size_t begin, size_t size)
        : owned(std::move(owned)), begin(begin), size(size) { }

    void Buffer::bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBindVertexArray(vao);
    }

    BufferRange Buffer::grab(size_t count) {
        if (allocated + count > vertices) {
            auto owned = std::make_unique<Buffer>(component, count);
            owned->allocated = count;

            return BufferRange(std::move(owned), 0, count);
        }

        size_t start = allocated;
        allocated += count;

        return BufferRange(this, start, count);
    }

    Buffer::Buffer(Child *component, size_t vertices) : Resource(component), vertices(vertices),
        buffer(glGenBuffers, glDeleteBuffers), vao(glGenVertexArrays, glDeleteVertexArrays) {

        bind();
        glBufferData(GL_ARRAY_BUFFER, vertices * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
        Vertex::mark();
    }

    Texture *TextureRange::get() const {
        return owned ? owned.get() : parent;
    }

    TextureRange &TextureRange::write(void *data) {
        get()->bind();

        glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);

        return *this;
    }

    TextureRange::TextureRange(Texture *parent, size_t x, size_t y, size_t w, size_t h)
        : parent(parent), x(x), y(y), w(w), h(h) { }
    TextureRange::TextureRange(std::unique_ptr<Texture> owned, size_t x, size_t y, size_t w, size_t h)
        : owned(std::move(owned)), x(x), y(y), w(w), h(h) { }

    TextureRange Texture::grab(size_t w, size_t h) {
        for (size_t x = 0; x < width; x++) {
            for (size_t y = 0; y < height; y++) {
                bool blocked = false;

                for (size_t ox = 0; ox < w; ox++) {
                    for (size_t oy = 0; oy < h; oy++) {
                        if (taken[(x + ox) + (y + oy) * width]) {
                            blocked = true;
                            break;
                        }
                    }

                    if (blocked)
                        break;
                }

                if (!blocked) {
                    for (size_t ox = 0; ox < w; ox++) {
                        for (size_t oy = 0; oy < h; oy++) {
                            taken[(x + ox) + (y + oy) * width] = true;
                        }
                    }

                    return TextureRange(this, x, y, w, h);
                }
            }
        }

        auto owned = std::make_unique<Texture>(component, w, h);
        std::fill(owned->taken.begin(), owned->taken.end(), true);

        return TextureRange(std::move(owned), 0, 0, w, h);
    }

    void Texture::bind() const {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindSampler(0, sampler);
    }

    Texture::Texture(Child *component, size_t width, size_t height) : Resource(component),
        width(height), height(height), taken(width * height),
        texture(glGenTextures, glDeleteTextures), sampler(glGenSamplers, glDeleteSamplers) {

        bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    void BoxVisual::set(float x, float y, float width, float height, const TextureRange &texture) {
        tex = &texture;

        range.write(shapes::square(x, y, width, height, texture).data());
    }

    void BoxVisual::set(const BoxBody &body, const TextureRange &texture) {
        auto pos = body.value->GetPosition();

        set(pos.x, pos.y, body.width, body.height, texture);
    }

    void BoxVisual::draw() {
        tex->get()->bind();
        range.draw();
    }

    BoxVisual::BoxVisual(Child *parent) : Child(parent), range(get<Buffer>(6)->grab(6)) { }

    bool BoxBody::isGround() const {
        return weight == 0;
    }

    void BoxBody::setVelocity(std::optional<float> x, std::optional<float> y) {
        auto velocity = value->GetLinearVelocity();

        if (x)
            velocity.x = *x;
        if (y)
            velocity.y = *y;

        value->SetLinearVelocity(velocity);
    }

    void BoxBody::capVelocity(std::optional<float> x, std::optional<float> y) {
        auto velocity = value->GetLinearVelocity();

        if (x)
            velocity.x = (velocity.x < 0 ? -1.0f : +1.0f) * std::min(std::abs(velocity.x), *x);
        if (y)
            velocity.y = (velocity.y < 0 ? -1.0f : +1.0f) * std::min(std::abs(velocity.y), *y);

        value->SetLinearVelocity(velocity);
    }

    BoxBody::BoxBody(Child *parent, float x, float y, float w, float h, float weight) : Child(parent), weight(weight) {
        width = w;
        height = h;

        b2BodyDef bDef;
        bDef.fixedRotation = true;
        bDef.position = b2Vec2(x, y);
        bDef.type = isGround() ? b2_staticBody : b2_dynamicBody;
        bDef.userData.pointer = reinterpret_cast<uintptr_t>(this);

        value = engine.world.CreateBody(&bDef);

        b2PolygonShape fShape;
        fShape.SetAsBox(width / 2 - 0.01f, height / 2 - 0.01f);

        b2FixtureDef fDef;
        fDef.shape = &fShape;
        fDef.density = weight;
        fDef.friction = 0.9;

        value->CreateFixture(&fDef);
    }

    BoxBody::~BoxBody() {
        engine.world.DestroyBody(value);
    }

    void Box::update(float time) {
        // No need to bother redrawing to buffer if its static.
        if (alive)
            visual->set(*body, texture);
    }

    Box::Box(Child *parent, float x, float y, float width, float height, Color color, float weight)
        : Child(parent), texture(std::move(get<parts::Texture>(1, 1)->grab(1, 1).write(color.data().data()))) {
        visual = make<BoxVisual>();
        body = make<BoxBody>(x, y, width, height, weight);

        alive = !body->isGround();

        visual->set(*body, this->texture);
    }

    Box::Box(Child *parent, float x, float y, float width, float height, TextureRange texture, float weight)
        : Child(parent), texture(std::move(texture)) {
        visual = make<BoxVisual>();
        body = make<BoxBody>(x, y, width, height, weight);

        alive = !body->isGround();

        visual->set(*body, this->texture);
    }

    namespace shapes {
        std::array<Vertex, 6> square(float x, float y, float width, float height, const TextureRange &texture) {
            Position bottomLeftPos = { x - width / 2, y - height / 2 };
            Position bottomRightPos = { x + width / 2, y - height / 2 };
            Position topLeftPos = { x - width / 2, y + height / 2 };
            Position topRightPos = { x + width / 2, y + height / 2 };

            float tw = texture.get()->width;
            float th = texture.get()->height;

            float shiftX = 1 / (tw * 100);
            float shiftY = 1 / (th * 100);

            Position bottomLeftTex = { texture.x / tw + shiftX, (texture.y + texture.h) / th - shiftY };
            Position bottomRightTex = { (texture.x + texture.w) / tw - shiftX, (texture.y + texture.h) / th - shiftY };
            Position topLeftTex = { texture.x / tw + shiftX, texture.y / th + shiftY };
            Position topRightTex = { (texture.x + texture.w) / tw - shiftX, texture.y / th + shiftY };

            Vertex bottomLeft = { bottomLeftPos, bottomLeftTex };
            Vertex bottomRight = { bottomRightPos, bottomRightTex };
            Vertex topLeft = { topLeftPos, topLeftTex };
            Vertex topRight = { topRightPos, topRightTex };

            return {
                bottomRight, topLeft, bottomLeft,
                bottomRight, topRight, topLeft
            };
        }
    }
}
