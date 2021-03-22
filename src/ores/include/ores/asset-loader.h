#pragma once

#include <ores/assets.h>

#include <string>
#include <unordered_map>

struct TagInfo {
    size_t start = 0, end = 0;
};

struct FrameInfo {
    size_t x = 0, y = 0, width = 0, height = 0;

    size_t duration = 100; // in ms
};

struct AssetLoader {
    assets::ImageData image;

    std::vector<FrameInfo> frames;
    std::unordered_map<std::string, TagInfo> tags;

    std::pair<size_t, size_t> size();

    AssetLoader(const std::string &path, const std::string &assets);
    AssetLoader(const std::pair<std::string, std::string> &data, const std::string &assets);
};
