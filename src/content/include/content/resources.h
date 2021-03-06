#pragma once

#include <engine/engine.h>

struct Map;
struct Font;
struct Flag;
struct Client;
struct Player;
struct Camera;
struct Options;

struct Resources : public Resource {
    const Options &options;

    Map *map = nullptr;
    Font *font = nullptr;
    Player *player = nullptr;
    Client *client = nullptr;
    Camera *camera = nullptr;

    std::unordered_map<std::string, Flag *> flags;

    Resources(Child *component, const Options &options);
};
