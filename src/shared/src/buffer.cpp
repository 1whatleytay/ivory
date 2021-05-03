#include <shared/buffer.h>

#include <cassert>

int64_t Reader::sizeLeft() const {
    return size - index;
}

Reader::Reader(const uint8_t *data, int64_t size) : data(data), size(size) { }

Reader::~Reader() {
    if (size != index)
        assert(false);
}
