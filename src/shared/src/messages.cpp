#include <shared/messages.h>

namespace messages {
    // messages::Hello
    MessageType Hello::type() const { return MessageType::Hello; }
    void Hello::write(Writer &writer) const {
        writer.write(playerId, playerX, playerY, worldWidth, worldHeight, blocks);
    }
    void Hello::read(Reader &reader) {
        reader.read(playerId, playerX, playerY, worldWidth, worldHeight, blocks);
    }

    Hello::Hello(size_t playerId, float playerX, float playerY,
        size_t worldWidth, size_t worldHeight, std::vector<blocks::BlockIndex> blocks)
        : playerId(playerId), playerX(playerX), playerY(playerY),
        worldWidth(worldWidth), worldHeight(worldHeight), blocks(std::move(blocks)) { }

    // messages::Move
    MessageType Move::type() const { return MessageType::Move; }
    void Move::read(Reader &reader) {
        reader.read(playerId, x, y);
    }
    void Move::write(Writer &writer) const {
        writer.write(playerId, x, y);
    }

    Move::Move(size_t playerId, float x, float y) : playerId(playerId), x(x), y(y) { }

    // messages::Replace
    MessageType Replace::type() const { return MessageType::Replace; }
    void Replace::read(Reader &reader) {
        reader.read(x, y, block);
    }
    void Replace::write(Writer &writer) const {
        writer.write(x, y, block);
    }

    Replace::Replace(size_t x, size_t y, int64_t block) : x(x), y(y), block(block) { }

    // messages::Log
    MessageType Log::type() const { return MessageType::Log; }
    void Log::read(Reader &reader) {
        reader.read(message);
    }
    void Log::write(Writer &writer) const {
        writer.write(message);
    }

    Log::Log(std::string message) : message(std::move(message)) { }

    MessageType Disconnect::type() const { return MessageType::Disconnect; }
    void Disconnect::read(Reader &reader) {
        reader.read(playerId);
    }
    void Disconnect::write(Writer &writer) const {
        writer.write(playerId);
    }

    Disconnect::Disconnect(size_t playerId) : playerId(playerId) { }
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
        case MessageType::Log:
            return get<messages::Log>(reader);
        case MessageType::Disconnect:
            return get<messages::Disconnect>(reader);

        default:
            throw std::exception();
    }
}
