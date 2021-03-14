#include <ores/game.h>

#include <ores/world.h>
#include <ores/camera.h>
#include <ores/player.h>

Game::Game(Engine &engine) : Child(engine) {
    supply<parts::Buffer>(600);
    supply<parts::Texture>(200, 200);

    make<Camera>();

    make<Player>(0, 2);

    make<World>();
}
