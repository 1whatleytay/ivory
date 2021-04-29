#include <content/assets.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <fmt/format.h>

#include <fstream>
#include <sstream>

namespace assets {
    ImageData::ImageData() : data(nullptr, stbi_image_free) { }

    ImageData loadImage(const std::string &path) {
        ImageData data;
        data.data.reset(stbi_load(path.c_str(), &data.width, &data.height, nullptr, 4));

        if (!data.data)
            throw std::runtime_error(fmt::format("Could not load texture at {}.", path));

        return data;
    }

    parts::TextureRange *load(parts::Texture &texture, const std::string &path) {
        return loadAbsolute(texture, (texture.component->engine.assets / path).string());
    }

    parts::TextureRange *loadAbsolute(parts::Texture &texture, const std::string &path) {
        ImageData data = loadImage(path);

        parts::TextureRange *range = texture.grab(data.width, data.height);
        range->write(data.data.get());

        return range;
    }

    std::string resolve(const Paths &paths) {
        auto test = std::get<0>(paths);

        if (fs::exists(test))
            return test;

        fs::path filename = fs::path(test).filename();

        auto relative = std::get<1>(paths);
        if (!relative.empty()) {
            test = (relative / filename).string();

            if (fs::exists(test))
                return test;
        }

        auto normal = std::get<2>(paths);
        if (!normal.empty()) {
            test = (normal / filename).string();

            if (fs::exists(test))
                return test;
        }

        throw std::runtime_error(fmt::format("Could not resolve image {}.", std::get<0>(paths)));
    }

    std::pair<std::string, std::string> loadResolved(const Paths &paths) {
        std::string p = resolve(paths);

        std::ifstream stream(p);
        std::stringstream buffer;
        buffer << stream.rdbuf();

        return { buffer.str(), p };
    }
}
