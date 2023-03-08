#pragma once

#include "raylib.h"

#include <functional>
#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include "engine/utility.hpp"
#include "engine/text.hpp"
#include "engine/scene.hpp"

// Ui components / interfaces

class UiComponent {
protected:
    // TODO: maybe rework to template, in case we will need to deal with some
    // things from specific node's implementation.
    // Or maybe use some CRTP magic to allow anything to be parent, for as long
    // as it implements component-specific things
    RectangleNode* parent;

public:
    UiComponent(RectangleNode* p) : parent(p) {}

    virtual void update(float) {}
    virtual void draw() {}
};

class TextComponent : public UiComponent {
protected:
    Text text = Text("");
    // According to raylib's sources, smallest size allowed by default is 10
    // Probably there is a reason for that?
    int size = DEFAULT_TEXT_SIZE;
    Color color = DEFAULT_TEXT_COLOR;
    Font font = GetFontDefault();
    int spacing = DEFAULT_TEXT_SPACING;

public:
    TextComponent(RectangleNode* p) : UiComponent(p) {}

    // TODO: test if changing these after init works correctly
    void set_text(Text txt) {
        text = txt;
        parent->set_size(text.get_size(font, size, spacing));
    }

    void set_text(const std::string& txt) {
        text.set_text(txt);
        // rect = text.get_rect();
        parent->set_size(text.get_size(font, size, spacing));
    }

    void set_size(int s) {
        size = s;
    }

    const Text& get_text() {
        return text;
    }

    void set_color(Color c) {
        color = c;
    }

    void set_spacing(int sp) {
        spacing = sp;
    }

    void set_font(Font f) {
        font = f;
    }

    std::string to_string() {
        return text.to_string();
    }

    void draw() override {
        text.draw(parent->get_world_pos(), font, size, spacing, color);
    }
};

