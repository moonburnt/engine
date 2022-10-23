#pragma once

#include "raylib.h"

#include <functional>
#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include "utility.hpp"
#include "text.hpp"
#include "scene.hpp"

// This will replace Label
// TODO: maybe not include Text, but inherit from it?
// Basic text node with non-editable text
class BasicTextNode : public RectangleNode {
protected:
    Text text;
    // According to raylib's sources, smallest size allowed by default is 10
    // Probably there is a reason for that?
    int text_size = DEFAULT_TEXT_SIZE;
    Color text_color = DEFAULT_TEXT_COLOR;
    Font font = GetFontDefault();
    int spacing = DEFAULT_TEXT_SPACING;

public:
    BasicTextNode(Text txt, Font _font, int _size, int _spacing, Color _color);
    BasicTextNode(Text txt);
    BasicTextNode(const std::string& txt);

    Text get_text();

    void draw() override;
};

// Flexible text node
class TextNode : public BasicTextNode {
public:
    void set_text(Text txt);
    void set_text(const std::string& txt);
};

// FrameCounter from utility. Reworked.
class FrameCounter : public BasicTextNode {
private:
    int fps_value = 0;

protected:
    const char* format = "FPS: %02i";

public:
    FrameCounter(const char* format, Font font, int size, int spacing, Color color);
    FrameCounter();

    void update(float) override;
};

// General button's concepts, detached from any particular form
class ButtonMixin {
public:
    enum class ButtonState {
        Idle,
        Hover,
        Pressed,
        Clicked,
        Disabled
    };

    void set_callback(ButtonMixin::ButtonState state, std::function<void()> cb);

    void set_idle();
    void set_hover();
    void set_pressed();
    void set_clicked();
    void set_disabled();

    ButtonMixin::ButtonState get_current_state();

protected:
    ButtonMixin::ButtonState current_state = ButtonMixin::ButtonState::Idle;

    /*
    General flow idea is the following:
    - We call set_{state}();
    - It performs check;
    - If current_state == state:
    - - Return and thats it
    - Else:
    - - call on_{state}, which performs common things that can be overriden via
    inheritance. For example - plays sound or changes texture.
    - - if exists - call callback, which can be set via set_callback() function
    and serves as a task that can be set for each particular button instance.
    This can be useful, if we want to assign specific buttons with specific
    actions directly instead of queuing their state changes.
    Be vary tho - some actions performed during update cycle may cause segfaults,
    thus design callbacks with caution.
    */

    std::unordered_map<ButtonMixin::ButtonState, std::function<void()>> callbacks;

    virtual void on_idle() {}
    virtual void on_hover() {}
    virtual void on_press() {}
    virtual void on_click() {}
    virtual void on_disable() {}
};

// Simple button to build upon
class Button : public ButtonMixin, public RectangleNode {
public:
    Button(Rectangle rect);
};
