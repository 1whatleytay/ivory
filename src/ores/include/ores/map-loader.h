#pragma once

#include <string>
#include <vector>

struct TilesetLoader {
    size_t firstId;

    TilesetLoader(size_t firstId, const std::string &path, const std::string &assets);
    TilesetLoader(size_t firstId, const std::string &data, const std::string &path, const std::string &assets);
};

struct Layer {
    std::string name;

    size_t width, height;
    std::vector<size_t> data;
};

struct MapLoader {
    size_t width = 0, height = 0;
    size_t tileWidth = 0, tileHeight = 0;

    std::vector<Layer> layers;
    std::vector<TilesetLoader> tilesets;

    // No data, will obviously regret later.
    MapLoader(const std::string &path, const std::string &assets);
    MapLoader(const std::string &data, const std::string &path, const std::string &assets);
};
