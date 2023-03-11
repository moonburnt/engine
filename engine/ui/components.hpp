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
        // For now, won't change parent's size coz it clashes with other
        // components. Maybe I will figure out the better solution for that l8r
        // TODO
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

// Buttons
enum class ButtonState {
    Idle,
    Hover,
    Pressed,
    Clicked,
    Disabled
};

class ButtonStateObserver {
public:
    virtual void update(float) = 0;
    virtual ~ButtonStateObserver() = default;
};

class ButtonStateSubject {
private:
    std::vector<std::shared_ptr<ButtonStateObserver>> observers = {};

public:
    void register_observer(ButtonStateObserver* o) {
        observers.push_back(std::shared_ptr<ButtonStateObserver>(o));
    }

    void remove_observer(ButtonStateObserver* o) {
        std::vector<std::shared_ptr<ButtonStateObserver>>::iterator it;

        it = find_if(
            observers.begin(),
            observers.end(),
            [](std::shared_ptr<ButtonStateObserver> i) { return i.get() == 0; }
        );

        if (it != observers.end()) {
            observers.at(std::distance(observers.begin(), it)) = nullptr;
        }
    }

    void notify_observers(float dt) {
        for (auto i: observers) {
            if (i != nullptr) {
                i->update(dt);
            }
        }

        observers.erase(
            std::remove_if(
                observers.begin(),
                observers.end(),
                [](std::shared_ptr<ButtonStateObserver> i){ return i.get() == nullptr; }
            ),
            observers.end()
        );

        // if (to_remove.size() > 0) {
        //     for (auto i = 0ul; i < to_remove.size(); i++) {
        //         std::vector<ButtonStateObserver*>::iterator it;

        //         it = find(observers.begin(), observers.end(), to_remove.at(i));

        //         if (it != observers.end()) {
        //             observers.erase(it);
        //         }

        //         ButtonStateObserver* n = observers.at(i);
        //         spdlog::info("rm observer");
        //         to_remove.at(i) = nullptr;
        //         delete n;
        //     }
        //     to_remove.clear();
        // }
    }
};

class ButtonComponent: public UiComponent {
private:
    ButtonState current_state = ButtonState::Idle;
    std::optional<ButtonState> future_state = std::nullopt;
    // std::unordered_map<ButtonState, std::function<void()>> callbacks;
    std::unordered_map<ButtonState, ButtonStateSubject> bs_handlers = {};

public:
    // This node should not necessary depend on Rectangle node type
    // #TODO
    ButtonComponent(RectangleNode* p) : UiComponent(p) {}

    const ButtonStateSubject& get_handler(ButtonState state) {
        // default-initialize in case it did not exist
        return bs_handlers[state];
    }

    // Schedule new state to be applied on next update() call
    void schedule_state_change(ButtonState state) {
        future_state = state;
    }

    void update(float dt) override {
        if (future_state == std::nullopt) {
            return;
        }

        if (future_state != current_state) {
            current_state = future_state.value();
            if (bs_handlers.find(current_state) != bs_handlers.end()) {
                bs_handlers.at(current_state).notify_observers(dt);
            }
        }
        future_state = std::nullopt;
    }
};
