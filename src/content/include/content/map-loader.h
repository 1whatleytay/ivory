#pragma once

#include <content/assets.h>

#include <string>
#include <vector>

struct TileProperties {
    bool solid = false;
    bool sneak = false;
};

struct TilesetLoader {
    std::string name;

    size_t tileWidth = 0, tileHeight = 0;

    assets::ImageData imageData;

    std::unordered_map<size_t, TileProperties> properties;

    TilesetLoader(const std::string &path, const std::string &parent, const std::string &assets);
    TilesetLoader(const std::pair<std::string, std::string> &data, const std::string &assets);
};

struct Layer {
    std::string name;

    size_t width = 0, height = 0;
    std::vector<size_t> data;
};

struct Object {
    float x, y;

    std::string type;
    std::string team;
};

struct MapLoader {
    size_t width = 0, height = 0;
    size_t tileWidth = 0, tileHeight = 0;

    int64_t playerLayer = -1;

    std::vector<Layer> layers;
    std::vector<Object> objects;
    std::vector<std::pair<size_t, TilesetLoader>> tilesets;

    std::unordered_map<size_t, TileProperties> properties;

    // No data, will obviously regret later.
    MapLoader(const std::string &name, const std::string &assets);
    MapLoader(const std::pair<std::string, std::string> &data, const std::string &assets);
};
