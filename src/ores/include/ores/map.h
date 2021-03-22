#pragma once

#include <ores/map-loader.h>

#include <engine/parts.h>

template <typename T>
using MapPtr = std::unique_ptr<T, std::function<void(T *)>>;

struct MapBody : MapPtr<b2Body> {
    MapBody();
};

struct MapJoint : MapPtr<b2Joint> {
    b2World &w;

    MapJoint(b2World &w);
};

struct Map : public Child {
    static constexpr float tileWidth = 1.0f;

    std::vector<parts::Texture *> textures;
    std::unordered_map<size_t, parts::TextureRange *> tiles;

    std::vector<parts::BufferRange *> layers;

    std::vector<Holder<parts::BoxBody>> bodies;

    MapBody frictionBody;
    MapJoint frictionJoint;

    void makeBodies(const MapLoader &m);

    void draw() override;

    Map(Child *parent, const std::string &path, b2Body *player);
};
