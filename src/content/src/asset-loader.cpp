#include <content/asset-loader.h>

#include <rapidjson/document.h>

#include <fmt/format.h>

const TagInfo &AssetLoader::findTag(const std::string &name) const {
    auto iterator = std::find_if(tags.begin(), tags.end(), [&name](const auto &tag) {
        return tag.name == name;
    });

    if (iterator == tags.end())
        throw std::runtime_error(fmt::format("Missing tag {} on asset.", name));

    return *iterator;
}

const TagInfo &AssetLoader::findTagIn(const std::string &name, const std::string &in) const {
    const TagInfo &parent = findTag(in);

    auto iterator = std::find_if(tags.begin(), tags.end(), [&name, &parent](const auto &tag) {
        return tag.name == name && parent.start <= tag.start && parent.end >= tag.end;
    });

    if (iterator == tags.end())
        throw std::runtime_error(fmt::format("Missing tag {} on asset (in {}).", name, in));

    return *iterator;
}

std::pair<size_t, size_t> AssetLoader::size() {
    if (frames.empty())
        throw std::runtime_error("Tried to evaluate size of asset with no frames.");

    size_t w = frames.front().width, h = frames.front().height;

    for (const auto &f : frames) {
        if (f.width != w || f.height != h)
            throw std::runtime_error("Inconsistent frame sizes for asset that needs singular frame size.");
    }

    return { w, h };
}

AssetLoader::AssetLoader(const std::string &path, const std::string &assets)
    : AssetLoader(assets::loadResolved({
        path,
        fs::path(path).parent_path().string(),
        (assets / fs::path("images")).string()
    }), assets) { }

AssetLoader::AssetLoader(const std::pair<std::string, std::string> &data, const std::string &assets) {
    rapidjson::Document doc;
    doc.Parse(data.first.c_str());

    auto &meta = doc["meta"];

    auto tagList = meta["frameTags"].GetArray();

    for (const auto &t : tagList) {
        assert(std::strcmp(t["direction"].GetString(), "forward") == 0);

        std::string name = t["name"].GetString();

        tags.push_back({
            name,
            static_cast<size_t>(t["from"].GetInt64()),
            static_cast<size_t>(t["to"].GetInt64())
        });
    }

    auto frameList = doc["frames"].GetArray();

    for (const auto &f : frameList) {
        auto &frameData = f["frame"];

        assert(!f["rotated"].GetBool());

        frames.push_back({
            static_cast<size_t>(frameData["x"].GetInt64()),
            static_cast<size_t>(frameData["y"].GetInt64()),
            static_cast<size_t>(frameData["w"].GetInt64()),
            static_cast<size_t>(frameData["h"].GetInt64()),

            static_cast<size_t>(f["duration"].GetInt64())
        });
    }

    auto imageName = meta["image"].GetString();
    image = assets::loadImage(assets::resolve({
        imageName,
        fs::path(std::get<1>(data)).parent_path().string(),
        (assets / fs::path("images")).string()
    }));
}
