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
}
