#pragma once

#include <shared/buffer.h>
#include <shared/blocks.h>

#include <variant>

enum class MessageType {
    Error,
    Hello,
    Move,
    Replace,
    Log,
    Disconnect
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

namespace messages {
    struct Hello : public Event {
        size_t playerId;
        float playerX, playerY;

        size_t worldWidth, worldHeight;
        std::vector<blocks::BlockIndex> blocks;

        MessageType type() const override;
        void read(Reader &reader) override;
        void write(Writer &writer) const override;

        Hello() = default;
        Hello(size_t playerId, float playerX, float playerY,
            size_t worldWidth, size_t worldHeight, std::vector<blocks::BlockIndex> blocks);
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

    struct Replace : public Event {
        size_t x, y;

        blocks::BlockIndex block;

        MessageType type() const override;
        void read(Reader &reader) override;
        void write(Writer &writer) const override;

        Replace() = default;
        Replace(size_t x, size_t y, blocks::BlockIndex block);
    };

    struct Log : public Event {
        std::string message;

        MessageType type() const override;
        void read(Reader &reader) override;
        void write(Writer &writer) const override;

        Log() = default;
        Log(std::string message);
    };

    struct Disconnect : public Event {
        size_t playerId;

        MessageType type() const override;
        void read(Reader &reader) override;
        void write(Writer &writer) const override;

        Disconnect() = default;
        Disconnect(size_t playerId);
    };
}

using Message = std::variant<
    messages::Hello,
    messages::Move,
    messages::Replace,
    messages::Log,
    messages::Disconnect
>;

Message parse(const Container &container, const uint8_t *data);
