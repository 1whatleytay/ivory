#include <ores/font.h>

#include <fmt/printf.h>

void FontText::set(const std::string &text, float x, float y) {
    if (text.empty()) {
        range = nullptr;
        return;
    }

    std::vector<Vertex> data(text.size() * 6);

    if (range && range->size != data.size())
        range->parent->free(range);

    if (!range)
        range = get<parts::Buffer>(data.size())->grab(data.size());

    auto &index = font->supportedCharacters;

    for (size_t a = 0; a < text.size(); a++) {
        char c = text[a];

        auto entry = std::find(index.begin(), index.end(), c);
        if (entry == index.end())
            throw std::exception();

        FontCharacter &character = font->characters[std::distance(index.begin(), entry)];

        std::array<Vertex, 6> vertices = parts::shapes::square(
            x + character.xOffset + character.width / 2, y + character.yOffset - character.height / 2,
            character.width, character.height,
            character.range
        );

        std::copy(vertices.begin(), vertices.end(), data.begin() + a * 6);

        x += character.xAdvance;
    }

    range->write(data.data());
}

void FontText::draw() {
    if (range) {
        font->texture->bind();

        range->draw();
    }
}

FontText::FontText(Child *parent) : Child(parent), font(find<Font>()) { }
FontText::FontText(Child *parent, Font *font) : Child(parent), font(font) { }

Font::Font(Child *component, const std::string &path, float points) : Resource(component) {
    float dpi = 96 * 4;

    if (FT_Init_FreeType(&library)
        || FT_New_Face(library, path.c_str(), 0, &face)
        || FT_Set_Char_Size(face, points * 64, 0, dpi, 0))
        throw std::exception();

    size_t cellSize = std::ceil(points / 72.0f * dpi);

    texture = component->assemble<parts::Texture>(
        cellSize * supportedCharacters.size(), cellSize);

    for (size_t a = 0; a < supportedCharacters.size(); a++) {
        char c = supportedCharacters[a];

        uint32_t i = FT_Get_Char_Index(face, c);

        if (FT_Load_Glyph(face, i, 0)
            || FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL))
            throw std::exception();

        auto width = face->glyph->bitmap.width;
        auto height = face->glyph->bitmap.rows;

        auto *range = texture->grab(width, height);

        std::vector<uint8_t> data(width * height * 4);

        std::fill(data.begin(), data.end(), 255);

        for (size_t x = 0; x < width; x++) {
            for (size_t y = 0; y < height; y++) {
                data[4 * (x + y * width) + 3] = face->glyph->bitmap.buffer[x + y * width];
            }
        }

        range->write(data.data());

        assert(face->glyph->advance.y == 0);

        characters[a] = {
            range,

            face->glyph->bitmap_left * pxToScreen, face->glyph->bitmap_top * pxToScreen,
            face->glyph->advance.x / 64 * pxToScreen,

            face->glyph->bitmap.width * pxToScreen, face->glyph->bitmap.rows * pxToScreen
        };
    }
}