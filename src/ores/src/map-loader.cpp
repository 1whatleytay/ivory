#include <ores/map-loader.h>

#include <pugixml.hpp>

#include <fmt/printf.h>

#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace {
    std::string load(const std::string &path) {
        std::ifstream stream(path);
        std::stringstream buffer;
        buffer << stream.rdbuf();

        return buffer.str();
    }

    std::string resolve(const std::string &path, const std::string &normalPath) {
        if (fs::exists(path))
            return path;

        std::string test = normalPath / fs::path(path).filename();

        return fs::exists(test) ? test : throw std::exception();
    }
}

TilesetLoader::TilesetLoader(size_t firstId, const std::string &path, const std::string &assets)
    : TilesetLoader(firstId, load(path), path, assets) { }

TilesetLoader::TilesetLoader(
    size_t firstId, const std::string &data, const std::string &path, const std::string &assets) : firstId(firstId) {


}

MapLoader::MapLoader(const std::string &path, const std::string &assets)
    : MapLoader(load(path), path, assets) { }

MapLoader::MapLoader(const std::string &data, const std::string &path, const std::string &assets) {
    pugi::xml_document doc;
    if (!doc.load_string(data.c_str()))
        throw std::exception();

    auto map = doc.child("map");

    width = map.attribute("width").as_ullong();
    height = map.attribute("width").as_ullong();

    tileWidth = map.attribute("tilewidth").as_ullong();
    tileHeight = map.attribute("tileheight").as_ullong();

    if (tileWidth != 16 && tileHeight != 16)
        fmt::print("Custom tile width and height, {} x {}.\n", tileWidth, tileHeight);

    assert(std::strcmp(map.attribute("orientation").as_string(), "orthogonal") == 0);
    assert(std::strcmp(map.attribute("renderorder").as_string(), "right-down") == 0);

    for (const auto &t : map.children("tileset")) {
        tilesets.emplace_back(t.attribute("firstgid").as_ullong(), t.attribute("source").as_string(), assets);
    }

    for (const auto &l : map.children("layer")) {
        const char *encoding = l.child("data").text().as_string();
        size_t len = strlen(encoding);

        std::vector<size_t> result;

        std::stringstream stream;
        for (size_t a = 0; a < len; a++) {
            char c = encoding[a];

            if (std::isdigit(c)) {
                stream << c;
            } else {
                std::string digits = stream.str();

                if (!digits.empty()) {
                    result.push_back(std::stoull(digits));

                    stream.str("");
                }
            }
        }

        size_t w = l.attribute("width").as_ullong();
        size_t h = l.attribute("height").as_ullong();

        assert(w * h == result.size());

        layers.push_back({ l.attribute("name").as_string(), w, h, std::move(result) });
    }
}