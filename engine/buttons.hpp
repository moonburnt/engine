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


// Lets break this down a bit.
// If we are going mvc, things should look this way:
// Button should only hold current state, future state and list of callbacks
// Controller works in update cycle.
//

// Model: data, specific to EXACT UI ITEM (states, etc)
// Controller: runs in update cycle, determines how to change the data
// View: has both updater and draw - runs after controller, determines the
// changes necessary to apply for the thing to work.


// In this case, view is essentially tied with Node, and everything else attached to it


// Controller updates general node state.
template <typename T> class ControllerBase {
protected:
    T host;
public:
    ControllerBase(T _host)
        : host(_host) {}

    virtual void update(float dt) = 0;
}

// Render draws specific part of visuals processes visuals-related changes.
// template <T> class RenderBase {
// protected:
//     T host;
// public:
//     RenderBase(T _host)
//         : host(_host) {}

//     virtual void update(float dt) {};

//     virtual void draw() {};
// }

// class UiNodeBase: public RectangleNode {
// protected:
//     ControllerBase<UiNodeBase*>* controller = nullptr;
//     std::vector<RenderBase<UiNodeBase*>*> renderers = {};
// public:

// }

class ButtonBase;

class ButtonMouseInputController : public ControllerBase<ButtonBase*> {
public:
    ButtonMouseController(ButtonBase* _host)
        : ControllerBase(_host) {}

    void update(float dt) override {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            host->set_future_state(ButtonBase::ButtonStates::pressed);
        }
        // if (CheckCollisionPointRec(GetMousePosition(), rect)) {
        //     if (last_state == ButtonStates::pressed) {
        //         if (IsMouseButtonUp(MOUSE_BUTTON_LEFT)) {
        //             state = ButtonStates::clicked;
        //             // if (on_click_callback != nullptr) {
        //             //     on_click_callback();
        //             // }
        //         }
        //     }
        //     else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        //         state = ButtonStates::pressed;
        //     }

        //     else {
        //         state = ButtonStates::hover;
        //     }
        // }
        // else {
        //     state = ButtonStates::idle;
        // }

        // if (state != last_state && sounds.count(state)) {
        //     PlaySound(*sounds[state]);
        // }

        // // Moved this there to avoid heap-use-after-free errors
        // if (state == ButtonStates::clicked && on_click_callback != nullptr) {
        //     on_click_callback();
        // }

        // last_state = state;

        // return state;
    }
};

enum class ButtonState {
    Idle,
    Hover,
    Pressed,
    Clicked,
    Disabled
};

class ButtonBase {
public:
    // bool set_state(ButtonState state) {
    //     if (current_state == state) {
    //         return false;
    //     }
    //     else {
    //         current_state = state;
    //         if (callbacks.count(state) > 0) {
    //             callbacks.at(state)();
    //         }
    //     }
    // }
    bool set_future_state(ButtonState state) {
        // Safety checs against non-nullopt future_state, maybe?
        if (current_state == state) {
            return false;
        }
        else {
            futute_state = state;
            return true;
        }
    }

    void set_callback(ButtonState state, std::function<void()> cb) {
        callbacks[state] = cb;
    };

    ButtonState get_current_state() {
        return current_state;
    }

    void update(float dt) {
        switch
    }

protected:
    std::unordered_map<ButtonState, std::function<void()>> callbacks = {};

    void apply_state_update() {
        if (future_state == std::nullopt) {
            return;
        }
        else {
            current_state = future_state;
            future_state = std::nullopt;
            if (callbacks.count(state) > 0) {
                callbacks.at(state)();
            }
        }
    }

private:
    ButtonState::current_state = ButtonState::Idle;
    std::optional<ButtonState> future_state = std::nullopt;
}
