#include <content/flag.h>

#include <content/assets.h>
#include <content/player.h>
#include <content/resources.h>

#include <fmt/format.h>

void Flag::update(float time) {
    if (holding) {
        auto pos = holding->flagPosition();

        visual->set(pos.x, pos.y, 0.35f, 0.35f, *texture);
    } else {
        auto pos = body->GetPosition();

        visual->set(pos.x, pos.y, 0.5, 0.5, *texture);
    }
}

void Flag::reset() {
    if (holding) {
        holding->holding = nullptr;
        holding = nullptr;

        body->SetTransform(b2Vec2(spawnX, spawnY), 0);
    }
}

void Flag::pickUp(FlagHoldable *by) {
    if (holding) {
        auto pos = holding->flagPosition();

        body->SetTransform(b2Vec2(pos.x, pos.y - 0.8f), 0);

        holding->holding = nullptr;
        holding = nullptr;
    }

    if (by) {
        holding = by;
        by->holding = this;
    }
}

Flag::Flag(Child *parent, std::string color, float x, float y)
    : Child(parent), color(std::move(color)), spawnX(x), spawnY(y) {

    auto *resources = find<Resources>();
    resources->flags[this->color] = this;

    texture = assets::load(*get<parts::Texture>(), fmt::format("images/objects/{}_flag.png", this->color));

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