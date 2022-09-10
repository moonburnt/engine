#include "text.hpp"
#include "spdlog/spdlog.h"

// Basic text
Text::Text(const std::string& _txt)
    : txt(_txt) {}

void Text::set_text(const std::string& _txt) {
    txt = _txt;
}

Vector2 Text::get_size(Font font, int size, int spacing) {
    return MeasureTextEx(font, txt.c_str(), size, spacing);
}

Vector2 Text::get_size() {
    return MeasureTextEx(
        DEFAULT_TEXT_FONT,
        txt.c_str(),
        DEFAULT_TEXT_SIZE,
        DEFAULT_TEXT_SPACING
    );
}

Rectangle Text::get_rect(Font font, int size, int spacing) {
    Vector2 txt_size = get_size(font, size, spacing);
    return {0.0f, 0.0f, txt_size.x, txt_size.y};
}

Rectangle Text::get_rect() {
    return get_rect(DEFAULT_TEXT_FONT, DEFAULT_TEXT_SIZE, DEFAULT_TEXT_SPACING);
}

void Text::draw(Vector2 pos, Font font, int size, int spacing, Color color) {
    // Not DrawTextPro, since its matrix-based thingy
    // Would rather apply it as additional method to use on top of visual objects
    DrawTextEx(font, txt.c_str(), pos, size, spacing, color);
}

void Text::draw(Vector2 pos) {
    draw(
        pos,
        DEFAULT_TEXT_FONT,
        DEFAULT_TEXT_SIZE,
        DEFAULT_TEXT_SPACING,
        DEFAULT_TEXT_COLOR
    );
}
