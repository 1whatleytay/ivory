#pragma once

#include <engine/engine.h>

struct Options {
    bool multiplayer = false;

    uint16_t port = 23788;
    std::string address = "127.0.0.1";

    std::string map = "map.tmx";
    std::string font = "Quicksand-Bold.ttf";

    std::string assetsPath;

    Options(int count, const char **args);
};
