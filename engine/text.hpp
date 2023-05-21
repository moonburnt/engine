#pragma once

#include "raylib.h"
#include <string>
#include <cstdint>

static constexpr uint32_t DEFAULT_TEXT_SIZE = 20u;
static constexpr Color DEFAULT_TEXT_COLOR = BLACK;
static constexpr uint32_t DEFAULT_TEXT_SPACING = DEFAULT_TEXT_SIZE / 10;
// static constexpr Font DEFAULT_TEXT_FONT = GetFontDefault();
// static Font DEFAULT_TEXT_FONT = GetFontDefault();

// Basic text class, include only necessary things.
// Maybe should merge into raybuff thingy later.
class Text {
protected:
    std::string txt;

public:
    Text(const std::string& _txt);

    void set_text(const std::string& _txt);

    std::string to_string() {
        return txt;
    }

    // Get text size
    Vector2 get_size(Font font, int size, int spacing);
    Vector2 get_size();
    // Get text rect
    Rectangle get_rect(Font font, int size, int spacing);
    Rectangle get_rect();

    void draw(Vector2 pos, Font font, int size, int spacing, Color color);
    virtual void draw(Vector2 pos);

};
