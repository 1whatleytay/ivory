#pragma once

#include <content/assets.h>

#include <string>
#include <unordered_map>

struct TagInfo {
    std::string name;
    size_t start = 0, end = 0;
};

struct FrameInfo {
    size_t x = 0, y = 0, width = 0, height = 0;

    size_t duration = 100; // in ms
};

struct AssetLoader {
    assets::ImageData image;

    std::vector<TagInfo> tags;
    std::vector<FrameInfo> frames;

    const TagInfo &findTag(const std::string &name) const;
    const TagInfo &findTagIn(const std::string &name, const std::string &in) const;

    std::pair<size_t, size_t> size();

    AssetLoader(const std::string &path, const std::string &assets);
    AssetLoader(const std::pair<std::string, std::string> &data, const std::string &assets);
};
