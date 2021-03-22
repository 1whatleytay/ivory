#include <ores/asset-loader.h>

#include <ores/assets.h>

#include <rapidjson/document.h>

std::pair<size_t, size_t> AssetLoader::size() {
    if (frames.empty())
        throw std::exception();

    size_t w = frames.front().width, h = frames.front().height;

    for (const auto &f : frames) {
        if (f.width != w || f.height != h)
            throw std::exception();
    }

    return { w, h };
}

AssetLoader::AssetLoader(const std::string &path, const std::string &assets)
    : AssetLoader(assets::loadResolved({ path, fs::path(path).parent_path(), assets / fs::path("images") }), assets) { }

AssetLoader::AssetLoader(const std::pair<std::string, std::string> &data, const std::string &assets) {
    rapidjson::Document doc;
    doc.Parse(data.first.c_str());

    auto &meta = doc["meta"];

    auto tagList = meta["frameTags"].GetArray();

    for (const auto &t : tagList) {
        assert(std::strcmp(t["direction"].GetString(), "forward") == 0);

        tags[t["name"].GetString()] = {
            static_cast<size_t>(t["from"].GetInt64()),
            static_cast<size_t>(t["to"].GetInt64())
        };
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
        imageName, fs::path(std::get<1>(data)).parent_path(), assets / fs::path("images")
    }));
}
