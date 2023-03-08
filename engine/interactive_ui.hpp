#pragma once

#include "node.hpp"
#include "ui.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include "spdlog/spdlog.h"
#include <optional>

// Interactive UI elements

enum class ButtonState {
    Idle,
    Hover,
    Pressed,
    Clicked,
    Disabled
};


cla

class Button {
private:
    ButtonState current_state = ButtonState::Idle;
    std::optional<ButtonState> future_state = std::nullopt;
    std::unordered_map<ButtonState, std::function<void()>> callbacks;

public:
    void set_callback(ButtonState state, std::function<void()> cb) {
        callbacks[state] = cb;
    }

    void update(float dt) {

    }

    void draw()
}



// // General button's concepts, detached from any particular form
// class ButtonMixin {
// public:
//     enum class ButtonState {
//         Idle,
//         Hover,
//         Pressed,
//         Clicked,
//         Disabled
//     };

//     void set_callback(ButtonMixin::ButtonState state, std::function<void()> cb);

//     void set_idle();
//     void set_hover();
//     void set_pressed();
//     void set_clicked();
//     void set_disabled();

//     ButtonMixin::ButtonState get_current_state();

// protected:
//     ButtonMixin::ButtonState current_state = ButtonMixin::ButtonState::Idle;

//     /*
//     General flow idea is the following:
//     - We call set_{state}();
//     - It performs check;
//     - If current_state == state:
//     - - Return and thats it
//     - Else:
//     - - call on_{state}, which performs common things that can be overriden via
//     inheritance. For example - plays sound or changes texture.
//     - - if exists - call callback, which can be set via set_callback() function
//     and serves as a task that can be set for each particular button instance.
//     This can be useful, if we want to assign specific buttons with specific
//     actions directly instead of queuing their state changes.
//     Be vary tho - some actions performed during update cycle may cause segfaults,
//     thus design callbacks with caution.
//     */

//     std::unordered_map<ButtonMixin::ButtonState, std::function<void()>> callbacks;

//     virtual void on_idle() {}
//     virtual void on_hover() {}
//     virtual void on_press() {}
//     virtual void on_click() {}
//     virtual void on_disable() {}
// };

// // Simple rectangle button to build upon
// class Button : public ButtonMixin, public virtual RectangleNode {
// public:
//     Button(Rectangle rect);
// };




// class UiNode : public Node {
// protected:

// }

// template <typename T> class InputController {
// protected:

// }
