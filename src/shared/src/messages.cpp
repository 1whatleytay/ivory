#include <shared/messages.h>

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
            return get<Hello>(reader);

        case MessageType::Move:
            return get<Move>(reader);

        default:
            throw std::exception();
    }
}
