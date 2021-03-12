#include <ores/game.h>

#include <ores/world.h>
#include <ores/camera.h>
#include <ores/player.h>

Game::Game(Engine &engine) : Child(engine) {
    create<parts::Buffer>(600);

    make<Camera>();

    make<Player>(0, 2);

    make<World>();
}
