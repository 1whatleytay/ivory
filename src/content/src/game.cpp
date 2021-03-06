#include <content/game.h>

#include <content/map.h>
#include <content/font.h>
#include <content/camera.h>
#include <content/player.h>
#include <content/client.h>
#include <content/options.h>
#include <content/resources.h>

#include <fmt/printf.h>

void Game::update(float time) {
    if (resources->client) {
        std::lock_guard guard(resources->client->messagesMutex);

        struct {
            Game& game;

            void operator()(const messages::Log& l) { throw std::exception(); }

            void operator()(const messages::Hello& h) {  }

            void operator()(const messages::Move& m) {
                assert(m.playerId != game.resources->client->hello.playerId);

                NetPlayer* player;

                auto iterator = game.netPlayers.find(m.playerId);
                if (iterator == game.netPlayers.end()) {
                    player = game.make<NetPlayer>(m.playerId, m.x, m.y);
                    game.netPlayers.insert({ m.playerId, player });

                    game.resources->map->addJoint(player->body->value);
                } else {
                    player = iterator->second;
                }

                player->handle(m);
            }

            void operator()(const messages::PickUp &p) {
                Flag *flag = game.resources->flags.at(p.color);

                if (p.letGo) {
                    flag->pickUp(nullptr);
                } else {
                    NetPlayer *player = game.netPlayers.at(p.playerId);

                    flag->pickUp(player);
                    flag->layerAfter(player);
                }

                flag->body->SetTransform(b2Vec2(p.x, p.y), 0);
            }

            void operator()(const messages::Capture &c) {
                Flag *flag = game.resources->flags.at(c.color);

                if (c.color == "blue")
                    game.resources->camera->redScore++;
                else if (c.color == "red")
                    game.resources->camera->blueScore++;

                flag->reset();
            }

            void operator()(const messages::SetHealth &k) {
                if (game.resources->client->hello.playerId == k.playerId) {
                    game.resources->player->die();
                }// else {
//                    auto iterator = game.netPlayers.find(k.playerId);
//                    assert(iterator != game.netPlayers.end());
                //}
            }

            void operator()(const messages::Disconnect& d) {
                assert(d.playerId != game.resources->client->hello.playerId);

                auto iterator = game.netPlayers.find(d.playerId);
                assert(iterator != game.netPlayers.end());

                iterator->second->drop(iterator->second->visual);
            }
        } visitor { *this };

        for (const Message& message : resources->client->messages)
            std::visit(visitor, message);

        resources->client->messages.clear();
    }
}

Game::Game(Engine &engine, const Options &options) : Child(engine) {
    resources = supply<Resources>(options);

    supply<parts::Buffer>(1200);
    supply<parts::Texture>(200, 200);

    resources->font = supply<Font>((engine.assets / "fonts" / options.font).string(), 24);

    if (options.multiplayer) {
        try {
            fmt::print("Connecting to server, {}:{}...", options.address, options.port);

            resources->client = supply<Client>(options.address, std::to_string(options.port));
            clientThread = std::make_unique<std::thread>([this]() { resources->client->run(); });

            while (!resources->client->hasHello);
        } catch (const std::exception &e) {
            throw;
        }
    }

    resources->camera = make<Camera>();
    resources->map = make<Map>((engine.assets / "maps" / options.map).string());

    resources->camera->redScoreText->layerTop();
    resources->camera->blueScoreText->layerTop();
}

Game::~Game() {
    if (resources->client) {
        resources->client->context.stop();
        clientThread->join();
    }
}
