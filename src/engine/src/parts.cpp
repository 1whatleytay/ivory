#include <engine/parts.h>

namespace parts {
    void Range::draw() const {
        get()->bind();

        glDrawArrays(GL_TRIANGLES, begin, size);
    }

    void Range::write(void *data) const {
        get()->bind();

        glBufferSubData(GL_ARRAY_BUFFER, begin * sizeof(Vertex), size * sizeof(Vertex), data);
    }

    Buffer *Range::get() const {
        return owned ? owned.get() : parent;
    }

    Range::Range(Buffer *parent, size_t begin, size_t size)
        : parent(parent), begin(begin), size(size) { }
    Range::Range(std::unique_ptr<Buffer> owned, size_t begin, size_t size)
        : owned(std::move(owned)), begin(begin), size(size) { }

    void Buffer::bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBindVertexArray(vao);
    }

    Range Buffer::grab(size_t count) {
        if (allocated + count > vertices)
            return Range(std::make_unique<Buffer>(component, count), 0, count);

        size_t start = allocated;
        allocated += count;

        return Range(this, start, count);
    }

    Buffer::Buffer(Child *component, size_t vertices) : Resource(component), vertices(vertices),
        buffer(glGenBuffers, glDeleteBuffers), vao(glGenVertexArrays, glDeleteVertexArrays) {

        bind();
        glBufferData(GL_ARRAY_BUFFER, vertices * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
        Vertex::mark();
    }

    void BoxVisual::set(float x, float y, float width, float height, const Color &color) const {
        range.write(shapes::square(x - width / 2, y - height / 2, width, height, color).data());
    }

    void BoxVisual::set(const BoxBody &body, const Color &color) const {
        auto pos = body.value->GetPosition();

        set(pos.x, pos.y, body.width, body.height, color);
    }

    void BoxVisual::draw() {
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
        if (!body->isGround())
            visual->set(*body, color);
    }

    Box::Box(Child *parent, float x, float y, float width, float height, Color color, float weight)
        : Child(parent), color(color) {
        visual = make<BoxVisual>();
        body = make<BoxBody>(x, y, width, height, weight);

        visual->set(*body, color);
    }
}
