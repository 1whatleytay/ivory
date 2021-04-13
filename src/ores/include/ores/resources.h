#pragma once

#include <engine/engine.h>

struct Font;
struct Client;
struct Player;
struct Options;

struct Resources : public Resource {
    const Options &options;

    Font *font = nullptr;
    Player *player = nullptr;
    Client *client = nullptr;

    Resources(Child *component, const Options &options);
};
