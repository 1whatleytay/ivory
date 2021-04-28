#include <shared/messages.h>

namespace messages {
    // messages::Hello
    MessageType Hello::type() const { return MessageType::Hello; }
    void Hello::write(Writer &writer) const {
        writer.write(playerId, color);
    }
    void Hello::read(Reader &reader) {
        reader.read(playerId,  color);
    }

    Hello::Hello(size_t playerId, std::string color)
        : playerId(playerId),  color(std::move(color)) { }

    // messages::Move
    MessageType Move::type() const { return MessageType::Move; }
    void Move::read(Reader &reader) {
        reader.read(playerId, x, y, veloX, veloY, animation);
    }
    void Move::write(Writer &writer) const {
        writer.write(playerId, x, y, veloX, veloY, animation);
    }

    Move::Move(size_t playerId, float x, float y, float veloX, float veloY, std::string animation)
        : playerId(playerId), x(x), y(y), veloX(veloX), veloY(veloY), animation(std::move(animation)) { }

    MessageType PickUp::type() const { return MessageType::PickUp; }
    void PickUp::read(Reader &reader) {
        reader.read(letGo, playerId, color, x, y);
    }
    void PickUp::write(Writer &writer) const {
        writer.write(letGo, playerId, color, x, y);
    }

    PickUp::PickUp(bool letGo, size_t playerId, std::string color, float x, float y)
        : letGo(letGo), playerId(playerId), color(std::move(color)), x(x), y(y) { }

    MessageType Capture::type() const { return MessageType::Capture; }
    void Capture::read(Reader &reader) {
        reader.read(color);
    }
    void Capture::write(Writer &writer) const {
        writer.write(color);
    }

    Capture::Capture(std::string color) : color(std::move(color)) { }

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
        case MessageType::PickUp:
            return get<messages::PickUp>(reader);
        case MessageType::Capture:
            return get<messages::Capture>(reader);
        case MessageType::Log:
            return get<messages::Log>(reader);
        case MessageType::Disconnect:
            return get<messages::Disconnect>(reader);

        default:
            throw std::exception();
    }
}
