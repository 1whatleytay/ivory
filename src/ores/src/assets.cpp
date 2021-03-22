#include <ores/assets.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace assets {
    ImageData::ImageData() : data(nullptr, stbi_image_free) { }

    ImageData loadImage(const std::string &path) {
        ImageData data;
        data.data.reset(stbi_load(path.c_str(), &data.width, &data.height, nullptr, 4));

        if (!data.data)
            throw std::exception();

        return data;
    }

    parts::TextureRange *load(parts::Texture &texture, const std::string &path) {
        return loadAbsolute(texture, texture.component->engine.assets / path);
    }

    parts::TextureRange *loadAbsolute(parts::Texture &texture, const std::string &path) {
        ImageData data = loadImage(path);

        parts::TextureRange *range = texture.grab(data.width, data.height);
        range->write(data.data.get());

        return range;
    }
}
