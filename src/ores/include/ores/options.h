#pragma once

#include <engine/engine.h>

struct Options {
    bool multiplayer = false;

    uint16_t port = 23788;
    std::string address = "127.0.0.1";

    size_t worldWidth = 40, worldHeight = 100;

    std::string assetsPath;

    Options(int count, const char **args);
};

struct OptionsResource : public Resource {
    const Options &value;

    OptionsResource(Child *component, const Options &options);
};
