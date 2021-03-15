#pragma once

#include <string>
#include <vector>
#include <unordered_map>

struct BlockType {
    std::string name;
    std::string path;

    bool solid = true;

    BlockType(std::string name, std::string path, bool solid = true);
};

namespace blocks {
    using Blocks = std::vector<const BlockType *>;
    using Indices = std::unordered_map<const BlockType *, int64_t>;

    constexpr float blockSize = 1.2;

    Blocks getBlocks();
    Indices getIndices(const Blocks &blocks = getBlocks());

    Blocks decode(const std::vector<int64_t> &blocks, const Blocks &index);
    std::vector<int64_t> encode(const Blocks &blocks, const Indices &index);

    Blocks generate(size_t width, size_t height);
}
