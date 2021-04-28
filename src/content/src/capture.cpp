#include <content/capture.h>

Capture::Capture(Child *parent, std::string color, float x, float y) : Child(parent), color(std::move(color)) {
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
