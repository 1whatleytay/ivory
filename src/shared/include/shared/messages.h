#pragma once

#include <shared/buffer.h>

#include <variant>

enum class MessageType : uint32_t {
    Error,
    Hello,
    Move,
    Capture,
    PickUp,
    SetHealth,
    Log,
    Disconnect
};

//struct Verify {
//    enum VerifyEnum { A, B, C };
//
//    int64_t n = sizeof(Verify);
//
//    int8_t a = sizeof(int8_t);
//    int16_t b = sizeof(int16_t);
//    int32_t c = sizeof(int32_t);
//    int64_t d = sizeof(int64_t);
//    int64_t x = sizeof(size_t);
//    VerifyEnum k = A;
//    int64_t y = sizeof(VerifyEnum);
//
//    bool operator==(const Verify &v) const;
//};

struct Container {
    MessageType type = MessageType::Error;
    uint64_t size = 0;
};

struct Event {
    virtual MessageType type() const = 0;
    virtual void read(Reader &reader) = 0;
    virtual void write(Writer &writer) const = 0;

    virtual ~Event() = default;
};

namespace messages {
    struct Hello : public Event {
        int64_t playerId = 0;

        std::string color;

        MessageType type() const override;
        void read(Reader &reader) override;
        void write(Writer &writer) const override;

        Hello() = default;
        Hello(int64_t playerId, std::string color);
    };

    struct Move : public Event {
        int64_t playerId = 0;

        float x = 0, y = 0;
        float velocityX = 0, velocityY = 0;

        std::string animation;

        MessageType type() const override;
        void read(Reader &reader) override;
        void write(Writer &writer) const override;

        Move() = default;
        Move(int64_t playerId, float x, float y, float veloX, float veloY, std::string animation);
    };

    struct PickUp : public Event {
        bool letGo = false;

        int64_t playerId = 0;
        std::string color;

        float x = 0, y = 0;

        MessageType type() const override;
        void read(Reader &reader) override;
        void write(Writer &writer) const override;

        PickUp() = default;
        PickUp(bool letGo, int64_t playerId, std::string color, float x, float y);
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
        int64_t playerId = 0;

        bool dies = false;

        MessageType type() const override;
        void read(Reader &reader) override;
        void write(Writer &writer) const override;

        SetHealth() = default;
        SetHealth(int64_t playerId, bool dies);
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
        int64_t playerId = 0;

        MessageType type() const override;
        void read(Reader &reader) override;
        void write(Writer &writer) const override;

        Disconnect() = default;
        explicit Disconnect(int64_t playerId);
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
