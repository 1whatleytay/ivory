#pragma once

#include <engine/parts.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <array>

// https://stackoverflow.com/a/33484394
template <std::size_t N, std::size_t ... Is>
constexpr std::array<char, N - 1> asArray(const char (&a)[N], std::index_sequence<Is...>)
{
    return {{a[Is]...}};
}

template <std::size_t N>
constexpr std::array<char, N - 1> asArray(const char (&a)[N])
{
    return asArray(a, std::make_index_sequence<N - 1>());
}

struct Font;

struct FontText : public Child {
    Font *font = nullptr;

    parts::BufferRange *range = nullptr;

    void set(const std::string &text, float x, float y);

    void draw() override;

    FontText(Child *parent);
    FontText(Child *parent, Font *font);
};

struct FontCharacter {
    parts::TextureRange *range = nullptr;

    float xOffset = 0, yOffset = 0;
    float xAdvance = 0;

    float width = 0, height = 0;
};

struct Font : public Resource {
    FT_Library library = nullptr;
    FT_Face face = nullptr;

    float pxToScreen = 1 / 192.0f;

    parts::Texture *texture = nullptr;

    static constexpr std::array supportedCharacters = asArray(
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "1234567890!@#$%^&*()[]{}<>,./\\:;\'\"`~|+-=_!?"
        " "
    );

    std::array<FontCharacter, supportedCharacters.size()> characters;

    Font(Child *component, const std::string &path, float points = 12);
};
