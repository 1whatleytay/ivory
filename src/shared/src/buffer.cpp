#include <shared/buffer.h>

#include <cassert>

size_t Reader::sizeLeft() const {
    return size - index;
}

Reader::Reader(const uint8_t *data, size_t size) : data(data), size(size) { }

Reader::~Reader() {
    if (size != index)
        assert(false);
}
