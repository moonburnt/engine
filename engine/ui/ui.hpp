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
#include "engine/raybuff.hpp"
#include "components.hpp"

// Temporary, everything is implemented in header, for the sake of simplicity.
// TODO: move things out to .cpp once its done

// This will replace Label

class UiText: public RectangleNode {
protected:
    TextComponent text_component = TextComponent(this);

public:
    // TODO: construction with all parts customizable
    UiText(Text txt)
        : RectangleNode({0.0f, 0.0f, 0.0f, 0.0f}) {
            text_component.set_text(txt);
        }
    UiText(const std::string& txt)
        : RectangleNode({0.0f, 0.0f, 0.0f, 0.0f}) {
            text_component.set_text(txt);
        }

    const TextComponent& get_text_component() {
        return text_component;
    }

    std::string get_string() {
        return text_component.to_string();
    }

    void draw() override {
        text_component.draw();
    }
};

class Button : public RectangleNode {
protected:
    ButtonComponent button_comp = ButtonComponent(this);
public:
    Button(Rectangle r) : RectangleNode(r) {
        add_tag("Button");
    }

    void set_subject(ButtonState state, ButtonStateSubject* sub) {
        button_comp.set_subject(state, sub);
    }

    ButtonStateSubject* get_subject(ButtonState state) {
        return button_comp.get_subject(state);
    }

    // TODO: move state handling to component
    void update(float dt) override {
        // ButtonState state = ButtonState::Idle;
        ButtonState state;

        if (CheckCollisionPointRec(GetMousePosition(), get_rect())) {
            if (button_comp.get_current_state() == ButtonState::Pressed) {
                if (IsMouseButtonUp(MOUSE_BUTTON_LEFT)) {
                    state = ButtonState::Clicked;
                }
                else {
                    state = ButtonState::Pressed;
                }
            }
            else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                state = ButtonState::Pressed;
            }

            else {
                state = ButtonState::Hover;
            }
        }
        else {
            state = ButtonState::Idle;
        }

        button_comp.schedule_state_change(state);
        button_comp.update(dt);
    }

    std::string to_string() override {
        return fmt::format(
            "{} at {}: {}",
            get_tag(),
            get_world_pos(),
            button_comp.get_current_state()
        );
    }
};

// TODO: consider inheriting some things from UiText
class TextButton : public Button {
protected:
    TextComponent text_component = TextComponent(this);

public:
    TextButton(Text txt)
        : Button({0.0f, 0.0f, 0.0f, 0.0f}) {
            text_component.set_text(txt);
        }
    TextButton(const std::string& txt)
        : Button({0.0f, 0.0f, 0.0f, 0.0f}) {
            text_component.set_text(txt);
        }

    void set_text(const std::string& _txt) {
        text_component.set_text(_txt);
    }

    void draw() override {
        text_component.draw();
    }
};


// TODO: group these into a single debug overlay with configurable verbosity lvls
class FrameCounter : public RectangleNode {
private:
    int fps_value = 0;

protected:
    TextComponent text_component = TextComponent(this);
    const char* format = "FPS: %02i";

public:
    // TODO: fully customizable settings
    // FrameCounter(const char* format, Font font, int size, int spacing, Color color);
    FrameCounter() : RectangleNode({0.0f, 0.0f, 0.0f, 0.0f}) {
        add_tag("FPS Counter");
    }

    void update(float) override {
        int new_fps = GetFPS();
        if (new_fps != fps_value) {
            fps_value = new_fps;
            text_component.set_text(TextFormat(format, fps_value));
        }
    }

    void draw() override {
        text_component.draw();
    }
};

// MWP. TODO: split into components
class MousePosReporter: public RectangleNode {
private:
    Vector2 mouse_pos = {0.0f, 0.0f};

protected:
    TextComponent text_comp = TextComponent(this);
    const char* format = "Cursor: (%02i, %02i)";

public:
    MousePosReporter() : RectangleNode({0.0f, 30.0f, 0.0f, 0.0f}) {
        add_tag("MousePosReporter");
    }

    void update(float) override {
        Vector2 new_pos = GetMousePosition();
        if (new_pos != mouse_pos) {
            mouse_pos = new_pos;
            text_comp.set_text(
                TextFormat(
                    format,
                    static_cast<int>(mouse_pos.x),
                    static_cast<int>(mouse_pos.y)
                )
            );
        }
    }

    void draw() override {
        text_comp.draw();
    }
};


class LayerStorage;

// This compiles, but I'm yet to see a successfull collision
class NodeInspector: public RectangleNode {
private:
    // Tree to report items from
    LayerStorage* report_from;

protected:
    TextComponent text_comp = TextComponent(this);

public:
    NodeInspector(LayerStorage* r)
    : RectangleNode({0.0f, 90.0f, 0.0f, 0.0f})
    , report_from(r) {
        add_tag("Node Inspector");
    }

    void update(float) override {
        // TODO: there should be a way to optimize this, won't bother for now.

        std::string desc = "";

        Scene* sc = report_from->get_current_or_future();

        // I'm not sure this works correctly since we don't transform any pos to
        // to camera, but will do for now. TODO
        Vector2 mouse_pos = GetMousePosition();

        if (sc != nullptr) {
            std::vector<Node*> collides = {};
            for (auto i: sc->get_children()) {
               if (i->collides_with(mouse_pos)) {
                    collides.push_back(i);
                }
            }

            if (!collides.empty()) {
                desc = "Highlighting:";

                for (auto i: collides) {
                    desc += ("\n"+ i->to_string());
                }
            }
        }

        text_comp.set_text(desc);
    }

    void draw() override {
        text_comp.draw();
    }
};
