#include <ores/map.h>

#include <ores/map-loader.h>

#include <fmt/printf.h>

Map::Map(Child *parent, const std::string &path) : Child(parent) {
    MapLoader loader(path, engine.assets);

    fmt::print("");
}
