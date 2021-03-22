#include <ores/map-loader.h>

#include <ores/assets.h>

#include <pugixml.hpp>

#include <fmt/printf.h>

#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

TilesetLoader::TilesetLoader(const std::string &path, const std::string &parent, const std::string &assets)
    : TilesetLoader(assets::loadResolved({
        path, fs::path(parent).parent_path(), assets / fs::path("tilesets")
    }), assets) { }

TilesetLoader::TilesetLoader(const std::pair<std::string, std::string> &data, const std::string &assets) {
    pugi::xml_document doc;
    if (!doc.load_string(std::get<0>(data).c_str()))
        throw std::exception();

    auto tileset = doc.child("tileset");

    tileWidth = tileset.attribute("tilewidth").as_ullong();
    tileHeight = tileset.attribute("tileheight").as_ullong();

//    size_t columns = tileset.attribute("columns").as_ullong();
//    size_t tileCount = tileset.attribute("tilecount").as_ullong();

    auto image = tileset.child("image");
    imageData = assets::loadImage(assets::resolve({
        image.attribute("source").as_string(),
        fs::path(std::get<1>(data)).parent_path(),
        assets / fs::path("tilesets")
    }));

    assert(imageData.width == image.attribute("width").as_ullong()
        && imageData.height == image.attribute("height").as_ullong());

    for (const auto &t : tileset.children("tile")) {
        TileProperties &props = properties[t.attribute("id").as_ullong()];

        for (const auto &e : t.child("properties").children("property")) {
            std::string propName = e.attribute("name").as_string(), propType = e.attribute("type").as_string();

            if (propName == "solid") {
                assert(propType == "bool");

                props.solid = e.attribute("value").as_bool();
            } else if (propName == "sneak") {
                assert(propType == "bool");

                props.sneak = e.attribute("value").as_bool();
            } else {
                throw std::exception();
            }
        }
    }
}

MapLoader::MapLoader(const std::string &name, const std::string &assets)
    : MapLoader(assets::loadResolved({
        name, fs::path(name).parent_path(), assets / fs::path("maps")
    }), assets) { }

MapLoader::MapLoader(const std::pair<std::string, std::string> &data, const std::string &assets) {
    pugi::xml_document doc;
    if (!doc.load_string(std::get<0>(data).c_str()))
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
        tilesets.push_back({
            t.attribute("firstgid").as_ullong(),
            TilesetLoader(t.attribute("source").as_string(), std::get<1>(data), assets)
        });
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

        assert(w * h == result.size() && width == w && height == h);

        layers.push_back({ l.attribute("name").as_string(), w, h, std::move(result) });
    }

    for (const auto &t : tilesets) {
        for (const auto &p : t.second.properties) {
            properties[t.first + p.first] = p.second;
        }
    }
}