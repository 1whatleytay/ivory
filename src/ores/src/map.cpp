#include <ores/map.h>

#include <ores/map-loader.h>

#include <fmt/printf.h>

MapBody::MapBody() : MapPtr<b2Body>(nullptr, [](b2Body *b) {
    b->GetWorld()->DestroyBody(b);
}) { }

MapJoint::MapJoint(b2World &w) : MapPtr<b2Joint>(nullptr, [this](b2Joint *j) {
    this->w.DestroyJoint(j);
}), w(w) { }

void Map::draw() {
    textures.front()->bind();

    for (auto b = layers.rbegin(); b != layers.rend(); b++)
        (*b)->draw();
}

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

    auto r = assemble<parts::Texture>()->grab(1, 1, Color(0x0000FF).data().data());

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
                x * tileWidth + (w - 1) * tileWidth / 2 , -y * tileWidth - h * tileWidth / 2 + tileWidth / 2,
                tileWidth * w, tileWidth * h));


            auto visual = make<parts::BoxVisual>();
            visual->set(x * tileWidth + (w - 1) * tileWidth / 2 , -y * tileWidth - h * tileWidth / 2 + tileWidth / 2,
                tileWidth * w, tileWidth * h, *r, -0.1);
        }
    }
}

Map::Map(Child *parent, const std::string &path, b2Body *player) : Child(parent), frictionJoint(engine.world) {
    MapLoader loader(path, engine.assets);

    assert(loader.tilesets.size() == 1); // basically, only one thing can be bound at one time

    for (const auto &t : loader.tilesets) {
        const assets::ImageData &data = t.second.imageData;
        parts::Texture *texture = assemble<parts::Texture>(data.width, data.height, data.data.get());
        textures.push_back(texture);

        auto tileset = texture->grabTileset(t.second.tileWidth, t.second.tileHeight);
        for (size_t a = 0; a < tileset.size(); a++)
            tiles[t.first + a] = tileset[a];
    }

    for (const auto &l : loader.layers) {
        std::vector<Vertex> vertices(l.width * l.height * 6);

        for (int64_t x = 0; x < l.width; x++) {
            for (int64_t y = 0; y < l.height; y++) {
                size_t i = l.data[x + y * l.width];

                auto j = tiles.find(i);
                auto exists = j != tiles.end();

                float size = exists ? tileWidth : 0;

                auto v = parts::shapes::square(x * tileWidth, -y * tileWidth,
                    size, size, exists ? j->second : nullptr);

                std::copy(v.begin(), v.end(), vertices.begin() + ((x + y * l.width) * 6));
            }
        }

        parts::Buffer *buffer = assemble<parts::Buffer>(vertices.size());
        layers.push_back(buffer->grab(vertices.size(), vertices.data()));
    }

    b2BodyDef bDef;
    bDef.type = b2_staticBody;
    bDef.position = b2Vec2();

    frictionBody.reset(engine.world.CreateBody(&bDef));

    b2PolygonShape fShape;
    fShape.SetAsBox(tileWidth * loader.width, tileWidth * loader.height);

    b2FixtureDef fDef;
    fDef.shape = &fShape;
    fDef.density = 0;
    fDef.friction = 0.5;

    frictionBody->CreateFixture(&fDef);

    b2FrictionJointDef jDef;
    jDef.Initialize(player, frictionBody.get(), b2Vec2_zero);

    jDef.maxForce = 10;
    jDef.maxTorque = 0;

    frictionJoint.reset(engine.world.CreateJoint(&jDef));

    makeBodies(loader);
}
