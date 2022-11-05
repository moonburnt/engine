#include "ui.hpp"

#include <algorithm>
#include <optional>
#include "raylib.h"
#include <string>
#include <stdexcept>
#include <tuple>
#include "raybuff.hpp"
#include "spdlog/spdlog.h"

// BasicTextNode
BasicTextNode::BasicTextNode(Text txt)
    // TODO: measure rectangle there and pass it to RectangleNode
    // : RectangleNode(text.get_rect())
    : RectangleNode({0.0f, 0.0f, 0.0f, 0.0f})
    , text(txt) {
    // rect = text.get_rect();
    size = text.get_size(font, text_size, spacing);
}

BasicTextNode::BasicTextNode(const std::string& txt)
    : BasicTextNode(Text(txt)) {}

BasicTextNode::BasicTextNode(
    Text txt, Font _font, int _size, int _spacing, Color _color)
    // TODO: get rid of copypaste
    : RectangleNode({0.0f, 0.0f, 0.0f, 0.0f})
    , text(txt)
    , text_size(_size)
    , text_color(_color)
    , font(_font)
    , spacing(_spacing) {
    // rect = text.get_rect();
    size = text.get_size(font, text_size, spacing);
}

Text BasicTextNode::get_text() {
    return text;
}

void BasicTextNode::draw() {
    // RectangleNode::draw();
    text.draw(get_world_pos(), font, text_size, spacing, text_color);
}

// TextNode
void TextNode::set_text(Text txt) {
    text = txt;
    // rect = text.get_rect();
    size = text.get_size(font, text_size, spacing);
}

void TextNode::set_text(const std::string& txt) {
    text.set_text(txt);
    // rect = text.get_rect();
    size = text.get_size(font, text_size, spacing);
}

// FPS counter
FrameCounter::FrameCounter(
    const char* _format, Font _font, int _size, int _spacing, Color _color)
    : BasicTextNode(Text(""), _font, _size, _spacing, _color)
    , format(_format) {}

FrameCounter::FrameCounter()
    : BasicTextNode("") {}

void FrameCounter::update(float) {
    fps_value = GetFPS();
    text.set_text(TextFormat(format, fps_value));
    // rect = text.get_rect();
    size = text.get_size(font, text_size, spacing);
}

// ButtonMixin

void ButtonMixin::set_callback(ButtonMixin::ButtonState state, std::function<void()>cb) {
    callbacks[state] = cb;
}

void ButtonMixin::set_idle() {
    if (current_state == ButtonMixin::ButtonState::Idle) {
        return;
    }
    else {
        current_state = ButtonMixin::ButtonState::Idle;
        on_idle();
        if (callbacks.count(ButtonMixin::ButtonState::Idle) > 0) {
            callbacks.at(ButtonMixin::ButtonState::Idle)();
        }
    }
}

void ButtonMixin::set_hover() {
    if (current_state == ButtonMixin::ButtonState::Hover) {
        return;
    }
    else {
        current_state = ButtonMixin::ButtonState::Hover;
        on_hover();
        if (callbacks.count(ButtonMixin::ButtonState::Hover) > 0) {
            callbacks.at(ButtonMixin::ButtonState::Hover)();
        }
    }
}

void ButtonMixin::set_pressed() {
    if (current_state == ButtonMixin::ButtonState::Pressed) {
        return;
    }
    else {
        current_state = ButtonMixin::ButtonState::Pressed;
        on_press();
        if (callbacks.count(ButtonMixin::ButtonState::Pressed) > 0) {
            callbacks.at(ButtonMixin::ButtonState::Pressed)();
        }
    }
}

void ButtonMixin::set_clicked() {
    if (current_state == ButtonMixin::ButtonState::Clicked) {
        return;
    }
    else {
        current_state = ButtonMixin::ButtonState::Clicked;
        on_click();
        if (callbacks.count(ButtonMixin::ButtonState::Clicked) > 0) {
            callbacks.at(ButtonMixin::ButtonState::Clicked)();
        }
    }
}

void ButtonMixin::set_disabled() {
    if (current_state == ButtonMixin::ButtonState::Disabled) {
        return;
    }
    else {
        current_state = ButtonMixin::ButtonState::Disabled;
        on_disable();
        if (callbacks.count(ButtonMixin::ButtonState::Disabled) > 0) {
            callbacks.at(ButtonMixin::ButtonState::Disabled)();
        }
    }
}

ButtonMixin::ButtonState ButtonMixin::get_current_state() {
    return current_state;
}

// Button
Button::Button(Rectangle rect)
    : RectangleNode(rect)
    , ButtonMixin() {}
