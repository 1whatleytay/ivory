#pragma once

#include <cstdint>
#include <cstdlib>

struct Options {
    bool noBreak = false;

    uint16_t port = 23788;
    size_t worldWidth = 40, worldHeight = 100;

    Options(int count, const char **args);
};
