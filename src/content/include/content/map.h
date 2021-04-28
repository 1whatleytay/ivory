#pragma once

#include <content/map-loader.h>

#include <engine/parts.h>

struct MapDrawLayer : public Child {
    parts::BufferRange *buffer = nullptr;
    parts::Texture *texture = nullptr;

    void draw() override;

    MapDrawLayer(Child *parent, parts::BufferRange *buffer, parts::Texture *texture);
};

struct Map : public Child {
    static constexpr float tileSize = 1.0f;

    std::vector<parts::Texture *> textures;
    std::unordered_map<size_t, parts::TextureRange *> tiles;

    std::vector<MapDrawLayer *> layers;

    std::vector<Holder<parts::BoxBody>> bodies;

    parts::BodyPtr frictionBody;
    parts::JointPtr frictionJoint;

    void makeBodies(const MapLoader &m);

    Map(Child *parent, const std::string &path);
};
