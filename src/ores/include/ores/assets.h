#pragma once

#include <engine/parts.h>

namespace assets {
    parts::TextureRange *load(parts::Texture &texture, const std::string &path);
    parts::TextureRange *loadAbsolute(parts::Texture &texture, const std::string &path);
}
