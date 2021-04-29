#pragma once

#include <shared/buffer.h>

#include <variant>

enum class MessageType {
    Error,
    Hello,
    Move,
    Capture,
    PickUp,
    SetHealth,
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
        size_t playerId = 0;

        std::string color;

        MessageType type() const override;
        void read(Reader &reader) override;
        void write(Writer &writer) const override;

        Hello() = default;
        Hello(size_t playerId, std::string color);
    };

    struct Move : public Event {
        size_t playerId = 0;

        float x = 0, y = 0;
        float velocityX = 0, velocityY = 0;

        std::string animation;
        bool flipX = false;

        MessageType type() const override;
        void read(Reader &reader) override;
        void write(Writer &writer) const override;

        Move() = default;
        Move(size_t playerId, float x, float y, float veloX, float veloY, std::string animation, bool flipX);
    };

    struct PickUp : public Event {
        bool letGo = false;

        size_t playerId = 0;
        std::string color;

        float x = 0, y = 0;

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

    struct SetHealth : public Event {
        size_t playerId = 0;

        bool dies = false;

        MessageType type() const override;
        void read(Reader &reader) override;
        void write(Writer &writer) const override;

        SetHealth() = default;
        SetHealth(size_t playerId, bool dies);
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
        size_t playerId = 0;

        MessageType type() const override;
        void read(Reader &reader) override;
        void write(Writer &writer) const override;

        Disconnect() = default;
        explicit Disconnect(size_t playerId);
    };
}

using Message = std::variant<
    messages::Hello,
    messages::Move,
    messages::Log,
    messages::Capture,
    messages::PickUp,
    messages::SetHealth,
    messages::Disconnect
>;

Message parse(const Container &container, const uint8_t *data);
