#include <shared/messages.h>

namespace messages {
    // messages::Hello
    MessageType Hello::type() const { return MessageType::Hello; }
    void Hello::write(Writer &writer) const {
        writer.write(playerId, playerX, playerY, map);
    }
    void Hello::read(Reader &reader) {
        reader.read(playerId, playerX, playerY, map);
    }

    Hello::Hello(size_t playerId, float playerX, float playerY, std::string map)
        : playerId(playerId), playerX(playerX), playerY(playerY), map(std::move(map)) { }

    // messages::Move
    MessageType Move::type() const { return MessageType::Move; }
    void Move::read(Reader &reader) {
        reader.read(playerId, x, y);
    }
    void Move::write(Writer &writer) const {
        writer.write(playerId, x, y);
    }

    Move::Move(size_t playerId, float x, float y) : playerId(playerId), x(x), y(y) { }

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
        case MessageType::Log:
            return get<messages::Log>(reader);
        case MessageType::Disconnect:
            return get<messages::Disconnect>(reader);

        default:
            throw std::exception();
    }
}
