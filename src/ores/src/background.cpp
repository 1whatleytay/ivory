#include <ores/background.h>

#include <ores/assets.h>
#include <ores/camera.h>

void Background::draw() {
    visual->draw();
}

void Background::update(float time) {
    auto bounds = engine.bounds();

    visual->set(
        bounds.x + bounds.width / 2 + engine.offsetX / 10, bounds.y - bounds.height / 2 + engine.offsetY / 10,
        bounds.width * 2, bounds.height * 2, range, 0.1);
}

Background::Background(Child *parent) : Child(parent),
    range(assets::load(*get<parts::Texture>(0, 0), "images/backgrounds/sky.png")) {

    visual = make<parts::BoxVisual>();
}
