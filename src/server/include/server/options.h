#pragma once

#include <cstdint>
#include <cstdlib>

struct Options {
    bool blocksBreakable = true;

    size_t worldWidth = 40, worldHeight = 100;

    Options(int count, const char **args);
};
