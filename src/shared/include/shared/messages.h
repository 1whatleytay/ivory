#pragma once

#include <shared/buffer.h>
#include <shared/blocks.h>

#include <variant>

enum class MessageType {
    Error,
    Hello,
    Move,
};

struct Container {
    MessageType type = MessageType::Error;
    size_t size = 0;
};

struct Event {
    virtual MessageType type() const = 0;
    virtual void read(Reader &reader) = 0;
    virtual void write(Writer &writer) const = 0;

    virtual ~Event() = default;
};

struct Hello : public Event {
    size_t playerId;
    float playerX, playerY;

    size_t worldWidth, worldHeight;
    std::vector<int64_t> blocks;

    MessageType type() const override;
    void read(Reader &reader) override;
    void write(Writer &writer) const override;

    Hello() = default;
    Hello(size_t playerId, float playerX, float playerY,
        size_t worldWidth, size_t worldHeight, std::vector<int64_t> blocks);
};

struct Move : public Event {
    size_t playerId;

    float x, y;

    MessageType type() const override;
    void read(Reader &reader) override;
    void write(Writer &writer) const override;

    Move() = default;
    Move(size_t playerId, float x, float y);
};

using Message = std::variant<Hello, Move>;

Message parse(const Container &container, const uint8_t *data);
