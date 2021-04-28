#pragma once

#include <engine/parts.h>

namespace assets {
    struct ImageData {
        using DataPtr = std::unique_ptr<uint8_t, void(*)(void *)>;

        int32_t width = 0, height = 0;

        DataPtr data;

        ImageData();
    };

    ImageData loadImage(const std::string &path);

    parts::TextureRange *load(parts::Texture &texture, const std::string &path);
    parts::TextureRange *loadAbsolute(parts::Texture &texture, const std::string &path);

    using Paths = const std::tuple<std::string, std::string, std::string>;

    // First -> raw path, second -> relative path, third -> normal path
    std::string resolve(const Paths &paths);
    std::pair<std::string, std::string> loadResolved(const Paths &paths);
}
