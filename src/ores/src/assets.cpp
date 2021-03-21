#include <ores/assets.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace assets {
    parts::TextureRange *load(parts::Texture &texture, const std::string &path) {
        return loadAbsolute(texture, texture.component->engine.assets / path);
    }

    parts::TextureRange *loadAbsolute(parts::Texture &texture, const std::string &path) {
        int w, h, c;

        uint8_t *data = stbi_load(path.c_str(), &w, &h, &c, 4);
        assert(data);

        parts::TextureRange *range = texture.grab(w, h);
        range->write(data);

        stbi_image_free(data);

        return range;
    }
}
