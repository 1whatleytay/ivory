#include <ores/map.h>

#include <ores/font.h>
#include <ores/flag.h>
#include <ores/player.h>
#include <ores/capture.h>
#include <ores/resources.h>

#include <fmt/printf.h>

void MapDrawLayer::draw() {
    texture->bind();
    buffer->draw();
}

MapDrawLayer::MapDrawLayer(Child *parent, parts::BufferRange *buffer, parts::Texture *texture)
    : Child(parent), buffer(buffer), texture(texture) { }

void Map::makeBodies(const MapLoader &m) {
    bodies.clear();

    auto a = [&m](size_t x, size_t y) { return x + y * m.width; };

    std::vector<bool> taken(m.width * m.height);
    std::fill(taken.begin(), taken.end(), true);

    for (const auto &v : m.layers) {
        assert(v.width == m.width && v.height == m.height);

        for (size_t x = 0; x < v.width; x++) {
            for (size_t y = 0; y < v.height; y++) {
                auto i = m.properties.find(v.data[a(x, y)]);

                if (i != m.properties.end() && i->second.solid)
                    taken[a(x, y)] = false;
            }
        }
    }

    for (int64_t y = 0; y < m.height; y++) {
        for (int64_t x = 0; x < m.width; x++) {
            int64_t w, h;

            // prelim to determine width
            for (w = 0; x + w < m.width; w++) {
                if (taken[a(x + w, y)])
                    break;
            }

            if (w < 1)
                continue;

            // prelim to determine height
            for (h = 1; y + h < m.height; h++) {
                bool done = false;

                for (size_t ow = 0; ow < w; ow++) {
                    if (taken[a(x + ow, y + h)]) {
                        done = true;
                        break;
                    }
                }

                if (done)
                    break;
            }

            for (size_t ow = 0; ow < w; ow++) {
                for (size_t oh = 0; oh < h; oh++) {
                    assert(!taken[a(x + ow, y + oh)]);

                    taken[a(x + ow, y + oh)] = true;
                }
            }

            // only one block long and tall
            bodies.push_back(hold<parts::BoxBody>(
                x * tileSize + (w - 1) * tileSize / 2 + 0.5f, -y * tileSize - h * tileSize / 2 + tileSize / 2 - 0.5f,
                tileSize * w, tileSize * h));
        }
    }
}

Map::Map(Child *parent, const std::string &path) : Child(parent), frictionJoint(engine.world) {
    Resources *resources = find<Resources>();

    MapLoader loader(path, engine.assets.string());

    assert(loader.tilesets.size() == 1); // basically, only one thing can be bound at one time

    for (const auto &t : loader.tilesets) {
        const assets::ImageData &data = t.second.imageData;
        auto *texture = assemble<parts::Texture>(data.width, data.height, data.data.get());
        textures.push_back(texture);

        auto tileset = texture->grabTileset(t.second.tileWidth, t.second.tileHeight);
        for (size_t a = 0; a < tileset.size(); a++)
            tiles[t.first + a] = tileset[a];
    }

    for (const auto &l : loader.layers) {
        std::vector<Vertex> vertices;
        vertices.reserve(l.width * l.height * 6);

        for (int64_t x = 0; x < l.width; x++) {
            for (int64_t y = 0; y < l.height; y++) {
                size_t i = l.data[x + y * l.width];

                auto j = tiles.find(i);
                auto exists = j != tiles.end();

                float size = exists ? tileSize : 0;

                auto v = parts::shapes::square(x * tileSize + 0.5f, -y * tileSize - 0.5f,
                    size, size, exists ? j->second : nullptr);

                vertices.insert(vertices.end(), v.begin(), v.end());
            }
        }

        auto *buffer = assemble<parts::Buffer>(vertices.size());
        layers.push_back(make<MapDrawLayer>(buffer->grab(vertices.size(), vertices.data()), textures.front()));
    }

    assert(!resources->player);

    for (const auto &o : loader.objects) {
        if (o.type == "spawn") {
            if (o.team == "red") {
                if (resources->player)
                    continue;

                resources->player = make<Player>(o.team, o.x, o.y);

                if (loader.playerLayer != -1) {
                    resources->player->layerAfter(layers[loader.playerLayer]);
                }
            }
        } else if (o.type == "flag") {
            make<Flag>(o.team, o.x, o.y);
            make<Capture>(o.team, o.x, o.y);
        } else {
            throw std::exception();
        }
    }

    b2BodyDef bDef;
    bDef.type = b2_staticBody;
    bDef.position = b2Vec2();

    frictionBody.reset(engine.world.CreateBody(&bDef));

    b2PolygonShape fShape;
    fShape.SetAsBox(tileSize * loader.width, tileSize * loader.height);

    b2FixtureDef fDef;
    fDef.shape = &fShape;
    fDef.density = 0;
    fDef.friction = 0.5;

    frictionBody->CreateFixture(&fDef);

    b2FrictionJointDef jDef;
    jDef.Initialize(resources->player->body->value, frictionBody.get(), b2Vec2_zero);

    jDef.maxForce = 10;
    jDef.maxTorque = 0;

    frictionJoint.reset(engine.world.CreateJoint(&jDef));

    makeBodies(loader);
}
