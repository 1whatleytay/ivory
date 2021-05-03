#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <cstring>
#include <stdexcept>

#include <fmt/printf.h>

struct Reader {
    const uint8_t *data = nullptr;
    int64_t index = 0;
    int64_t size = 0;

    template <typename T>
    void add(T &value) {
        std::memcpy(&value, data + index, sizeof(T));

        index += sizeof(T);

        if (index > size)
            throw std::runtime_error("Parser read too much.");
    }

    template <typename T>
    void add(std::vector<T> &value) {
        value.clear();

        int64_t length;
        add(length);

        value.reserve(length);

        for (int64_t a = 0; a < length; a++) {
            T t;
            add(t);

            value.push_back(t);
        }
    }

    void add(std::string &value) {
        value.clear();

        int64_t length;
        add(length);

        value = std::string(reinterpret_cast<const char *>(&data[index]), length);
        index += length;
    }

    void read() { }

    template <typename T, typename ...Args>
    void read(T &t, Args & ...args) {
        add(t);

        read(args...);
    }

    int64_t sizeLeft() const;

    explicit Reader(const uint8_t *data, int64_t size);
    ~Reader();
};

struct Writer {
    std::vector<uint8_t> data = { };

    template <typename T>
    void add(const T &value) {
        auto *valueData = reinterpret_cast<const uint8_t *>(&value);

        data.insert(data.end(), valueData, valueData + sizeof(T));
    }

    template <typename T>
    void add(const std::vector<T> &value) {
        add<int64_t>(value.size());

        for (const T &x : value)
            add(x);
    }

    void add(const std::string &value) {
        add<int64_t>(value.size());

        data.insert(data.end(), value.begin(), value.end());
    }

    void write() { }

    template <typename T, typename ...Args>
    void write(const T &t, const Args & ...args) {
        add(t);

        write(args...);
    }
};
