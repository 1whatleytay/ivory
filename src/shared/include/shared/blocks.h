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
    extern BlockType wood;

    using BlockIndex = int32_t;

    using Blocks = std::vector<const BlockType *>;
    using Indices = std::unordered_map<const BlockType *, BlockIndex>;

    constexpr float blockSize = 1.2;

    Blocks getBlocks();
    Indices getIndices(const Blocks &blocks = getBlocks());

    Blocks decode(const std::vector<BlockIndex> &blocks, const Blocks &index);
    std::vector<BlockIndex> encode(const Blocks &blocks, const Indices &index);

    Blocks generate(size_t width, size_t height);
}
