#pragma once

#include <ores/map-loader.h>

#include <engine/parts.h>

struct Map : public Child {
    static constexpr float tileSize = 1.0f;

    std::vector<parts::Texture *> textures;
    std::unordered_map<size_t, parts::TextureRange *> tiles;

    std::vector<parts::BufferRange *> layers;

    std::vector<Holder<parts::BoxBody>> bodies;

    parts::BodyPtr frictionBody;
    parts::JointPtr frictionJoint;

    void makeBodies(const MapLoader &m);

    void draw() override;

    Map(Child *parent, const std::string &path);
};
