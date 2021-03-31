#include <ores/flag.h>

#include <ores/assets.h>

#include <fmt/format.h>

void Flag::update(float time) {
    auto pos = body->GetPosition();

    if (holdingPlayer)
        visual->set(pos.x, pos.y, 0.4f, 0.4f, *texture, -0.600);
    else
        visual->set(pos.x, pos.y, 1, 1, *texture, -0.125);
}

Flag::Flag(Child *parent, std::string color, float x, float y) : Child(parent), color(std::move(color)) {
    texture = assets::load(*get<parts::Texture>(0, 0), fmt::format("images/objects/{}_flag.png", this->color));

    visual = make<parts::BoxVisual>();

    b2BodyDef bDef;
    bDef.type = b2_staticBody;
    bDef.position = b2Vec2(x, y);

    user = this;
    bDef.userData.pointer = reinterpret_cast<uintptr_t>(&user);

    body.reset(engine.world.CreateBody(&bDef));

    b2PolygonShape fShape;
    fShape.SetAsBox(0.5f, 0.5f);

    b2FixtureDef fDef;
    fDef.isSensor = true;
    fDef.shape = &fShape;
    fDef.density = 0;

    body->CreateFixture(&fDef);
}