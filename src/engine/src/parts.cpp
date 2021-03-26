#include <engine/parts.h>

namespace parts {
    void BufferRange::draw() const {
        parent->bind();

        glDrawArrays(GL_TRIANGLES, begin, size);
    }

    BufferRange &BufferRange::write(void *data) {
        parent->bind();

        glBufferSubData(GL_ARRAY_BUFFER, begin * sizeof(Vertex), size * sizeof(Vertex), data);

        return *this;
    }

    BufferRange::BufferRange(Buffer *parent, size_t begin, size_t size)
        : parent(parent), begin(begin), size(size) { }

    void Buffer::bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBindVertexArray(vao);
    }

    BufferRange *Buffer::grab(size_t count, void *data) {
        if (allocated + count > vertices) {
            auto owned = std::make_unique<Buffer>(component, count);
            owned->allocated = count;

            auto range = std::make_unique<BufferRange>(owned.get(), 0, count);
            auto *ptr = range.get();

            if (data)
                range->write(data);

            children.push_back(std::make_pair(std::move(owned), std::move(range)));

            return ptr;
        }

        size_t start = allocated;
        allocated += count;

        auto range = std::make_unique<BufferRange>(this, start, count);
        auto *ptr = range.get();

        if (data)
            range->write(data);

        children.push_back(std::make_pair(nullptr, std::move(range)));

        return ptr;
    }

    Buffer::Buffer(Child *component, size_t vertices) : Resource(component), vertices(vertices),
        buffer(glGenBuffers, glDeleteBuffers), vao(glGenVertexArrays, glDeleteVertexArrays) {

        bind();
        glBufferData(GL_ARRAY_BUFFER, vertices * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
        Vertex::mark();
    }

    TextureRange &TextureRange::write(void *data) {
        parent->bind();

        glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);

        return *this;
    }

    TextureRange::TextureRange(Texture *parent, size_t x, size_t y, size_t w, size_t h)
        : parent(parent), x(x), y(y), w(w), h(h) { }

    TextureRange *Texture::grab(size_t x, size_t y, size_t w, size_t h) {
        for (int64_t ox = 0; ox < w; ox++) {
            for (int64_t oy = 0; oy < h; oy++) {
                taken[(x + ox) + (y + oy) * width] = true;
            }
        }

        auto range = std::make_unique<TextureRange>(this, x, y, w, h);

        auto *ptr = range.get();
        children.push_back({ nullptr, std::move(range) });

        return ptr;
    }

    TextureRange *Texture::grab(size_t w, size_t h, void *data) {
        for (int64_t x = 0; x < static_cast<int64_t>(width) - static_cast<int64_t>(w) + 1; x++) {
            for (int64_t y = 0; y < static_cast<int64_t>(height) - static_cast<int64_t>(h) + 1; y++) {
                bool blocked = false;

                for (int64_t ox = 0; ox < w; ox++) {
                    for (int64_t oy = 0; oy < h; oy++) {
                        if (taken[(x + ox) + (y + oy) * width]) { // this is fucked
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

                    auto range = std::make_unique<TextureRange>(this, x, y, w, h);

                    if (data)
                        range->write(data);

                    auto *ptr = range.get();
                    children.push_back({ nullptr, std::move(range) });

                    return ptr;
                }
            }
        }

        auto owned = std::make_unique<Texture>(component, w, h);
        std::fill(owned->taken.begin(), owned->taken.end(), true);

        auto range = std::make_unique<TextureRange>(owned.get(), 0, 0, w, h);
        if (data)
            range->write(data);

        auto *ptr = range.get();
        children.push_back({ std::move(owned), std::move(range) });

        return ptr;
    }

    std::vector<TextureRange *> Texture::grabTileset(size_t w, size_t h) {
        if (std::any_of(taken.begin(), taken.end(), [](bool a) { return a; }))
            throw std::exception();

        if (width % w != 0 || height % h != 0)
            throw std::exception();

        std::vector<TextureRange *> result;

        for (size_t y = 0; y < height; y += h) {
            for (size_t x = 0; x < width; x += w) {
                auto range = std::make_unique<TextureRange>(this, x, y, w, h);
                result.push_back(range.get());

                children.push_back({ nullptr, std::move(range) });
            }
        }

        std::fill(taken.begin(), taken.end(), true);

        return result;
    }

    void Texture::bind() const {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindSampler(0, sampler);
    }

    Texture::Texture(Child *component) : Texture(component, 0, 0) { }

    Texture::Texture(Child *component, size_t width, size_t height, void *data) : Resource(component),
        width(width), height(height), taken(width * height),
        texture(glGenTextures, glDeleteTextures), sampler(glGenSamplers, glDeleteSamplers) {

        bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    void BoxVisual::set(float x, float y, float width, float height, const TextureRange &texture, float depth) {
        tex = &texture;

        range->write(shapes::square(x, y, width, height, &texture, depth).data());
    }

    void BoxVisual::set(const BoxBody &body, const TextureRange &texture, float depth) {
        auto pos = body.value->GetPosition();

        set(pos.x, pos.y, body.width, body.height, texture, depth);
    }

    void BoxVisual::draw() {
        tex->parent->bind();
        range->draw();
    }

    BoxVisual::BoxVisual(Child *parent) : Child(parent), range(get<Buffer>(6)->grab(6)) { }

    bool BoxBody::isGround() const {
        return weight == 0;
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
        fDef.friction = isGround() ? 0 : 0.9;

        value->CreateFixture(&fDef);
    }

    BoxBody::~BoxBody() {
        engine.world.DestroyBody(value);
    }

    void Box::update(float time) {
        // No need to bother redrawing to buffer if its static.
        if (alive)
            visual->set(*body, *texture, depth);
    }

    Box::Box(Child *parent, float x, float y, float width, float height, Color color, float weight)
        : Child(parent), texture(get<parts::Texture>(1, 1)->grab(1, 1, color.data().data())) {
        visual = make<BoxVisual>();
        body = make<BoxBody>(x, y, width, height, weight);

        alive = !body->isGround();

        visual->set(*body, *this->texture);
    }

    Box::Box(Child *parent, float x, float y, float width, float height, TextureRange *texture, float weight)
        : Child(parent), texture(texture) {
        visual = make<BoxVisual>();
        body = make<BoxBody>(x, y, width, height, weight);

        alive = !body->isGround();

        visual->set(*body, *this->texture);
    }

    namespace shapes {
        std::array<Vertex, 6> square(float x, float y, float width, float height,
            const TextureRange *texture, float depth) {
            Vec3 bottomLeftPos = { x - width / 2, y - height / 2, depth };
            Vec3 bottomRightPos = { x + width / 2, y - height / 2, depth };
            Vec3 topLeftPos = { x - width / 2, y + height / 2, depth };
            Vec3 topRightPos = { x + width / 2, y + height / 2, depth };

            float tW = texture ? texture->parent->width : 1;
            float tH = texture ? texture->parent->height : 1;

            float shiftX = 1 / (tW * 100);
            float shiftY = 1 / (tH * 100);

            float sX = texture ? texture->x : 0, sY = texture ? texture->y : 0;
            float sW = texture ? texture->w : 0, sH = texture ? texture->h : 0;

            Vec2 bottomLeftTex = { sX / tW + shiftX, (sY + sH) / tH - shiftY };
            Vec2 bottomRightTex = { (sX + sW) / tW - shiftX, (sY + sH) / tH - shiftY };
            Vec2 topLeftTex = { sX / tW + shiftX, sY / tH + shiftY };
            Vec2 topRightTex = { (sX + sW) / tW - shiftX, sY / tH + shiftY };

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
