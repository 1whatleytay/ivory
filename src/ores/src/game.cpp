#include <ores/game.h>

#include <ores/world.h>
#include <ores/camera.h>
#include <ores/player.h>
#include <ores/client.h>

void Game::update(float time) {
    if (client) {
        std::lock_guard guard(client->messagesMutex);

        struct {
            Game& game;

            void operator()(const messages::Log& l) { throw std::exception(); }

            void operator()(const messages::Hello& h) {  }

            void operator()(const messages::Move& m) {
                assert(m.playerId != game.client->hello.playerId);

                NetPlayer* player;

                auto iterator = game.netPlayers.find(m.playerId);
                if (iterator == game.netPlayers.end()) {
                    player = game.make<NetPlayer>(m.playerId);
                    game.netPlayers.insert({ m.playerId, player });
                }
                else {
                    player = iterator->second;
                }

                player->x = m.x;
                player->y = m.y;
            }
            void operator()(const messages::Disconnect& d) {
                assert(d.playerId != game.client->hello.playerId);

                auto iterator = game.netPlayers.find(d.playerId);
                assert(iterator != game.netPlayers.end());

                iterator->second->drop(iterator->second->visual);
            }

            void operator()(const messages::Replace& r) {
                game.world->editBlock(r.x, r.y, r.block);
            }
        } visitor { *this };

        for (const Message& message : client->messages)
            std::visit(visitor, message);
    }
}

Game::Game(Engine &engine) : Child(engine) {
    supply<parts::Buffer>(600);
    supply<parts::Texture>(200, 200);

    try {
        client = supply<Client>("127.0.0.1");
        clientThread = std::make_unique<std::thread>([this]() { client->run(); });

        while (!client->hasHello);
    } catch (const std::exception &e) {

    }

    make<Camera>()->player = make<Player>();

    world = make<World>();
}

Game::~Game() {
    if (client) {
        client->context.stop();
        clientThread->join();
    }
}
