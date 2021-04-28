#pragma once

#include <shared/buffer.h>

#include <variant>

enum class MessageType {
    Error,
    Hello,
    Move,
    Capture,
    PickUp,
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

        std::string color;

        MessageType type() const override;
        void read(Reader &reader) override;
        void write(Writer &writer) const override;

        Hello() = default;
        Hello(size_t playerId, std::string color);
    };

    struct Move : public Event {
        size_t playerId;

        float x, y;
        float veloX, veloY;

        std::string animation;

        MessageType type() const override;
        void read(Reader &reader) override;
        void write(Writer &writer) const override;

        Move() = default;
        Move(size_t playerId, float x, float y, float veloX, float veloY, std::string animation);
    };

    struct PickUp : public Event {
        bool letGo = false;

        size_t playerId = 0;
        std::string color;

        float x, y;

        MessageType type() const override;
        void read(Reader &reader) override;
        void write(Writer &writer) const override;

        PickUp() = default;
        PickUp(bool letGo, size_t playerId, std::string color, float x, float y);
    };

    struct Capture : public Event {
        std::string color;

        MessageType type() const override;
        void read(Reader &reader) override;
        void write(Writer &writer) const override;

        Capture() = default;
        Capture(std::string color);
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
    messages::Log,
    messages::Capture,
    messages::PickUp,
    messages::Disconnect
>;

Message parse(const Container &container, const uint8_t *data);
