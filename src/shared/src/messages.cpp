#include <shared/messages.h>

namespace messages {
    MessageType Hello::type() const { return MessageType::Hello; }

    void Hello::write(Writer &writer) const {
        writer.write(playerId, playerX, playerY, worldWidth, worldHeight, blocks);
    }

    void Hello::read(Reader &reader) {
        reader.read(playerId, playerX, playerY, worldWidth, worldHeight, blocks);
    }

    Hello::Hello(size_t playerId, float playerX, float playerY,
        size_t worldWidth, size_t worldHeight, std::vector<int64_t> blocks)
        : playerId(playerId), playerX(playerX), playerY(playerY),
        worldWidth(worldWidth), worldHeight(worldHeight), blocks(std::move(blocks)) { }

    MessageType Move::type() const { return MessageType::Move; }

    void Move::read(Reader &reader) {
        reader.read(playerId, x, y);
    }

    void Move::write(Writer &writer) const {
        writer.write(playerId, x, y);
    }

    Move::Move(size_t playerId, float x, float y) : playerId(playerId), x(x), y(y) { }

    MessageType Replace::type() const { return MessageType::Replace; }

    void Replace::read(Reader &reader) {
        reader.read(x, y, block);
    }

    void Replace::write(Writer &writer) const {
        writer.write(x, y, block);
    }

    Replace::Replace(size_t x, size_t y, int64_t block) : x(x), y(y), block(block) { }
}

template <typename T>
T get(Reader &reader) {
    T t;
    t.read(reader);
    return t;
}

Message parse(const Container &container, const uint8_t *data) {
    Reader reader(data, container.size);

    switch (container.type) {
        case MessageType::Hello:
            return get<messages::Hello>(reader);

        case MessageType::Move:
            return get<messages::Move>(reader);

        case MessageType::Replace:
            return get<messages::Replace>(reader);

        default:
            throw std::exception();
    }
}
