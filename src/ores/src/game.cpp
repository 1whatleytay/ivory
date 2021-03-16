#include <ores/game.h>

#include <ores/world.h>
#include <ores/camera.h>
#include <ores/player.h>
#include <ores/client.h>

Game::Game(Engine &engine) : Child(engine) {
    supply<parts::Buffer>(600);
    supply<parts::Texture>(200, 200);

    try {
        client = supply<Client>("127.0.0.1");
        clientThread = std::make_unique<std::thread>([this]() { client->run(); });
    } catch (const std::exception &e) {
        throw;
    }

    make<Camera>()->player = make<Player>();

    make<World>();
}

Game::~Game() {
    if (client) {
        client->kill = true;
        clientThread->join();
    }
}
