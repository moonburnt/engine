#include "ui.hpp"


// UiText
UiText::UiText(Text txt)
    : RectangleNode({0.0f, 0.0f, 0.0f, 0.0f}) {
        text_component.set_text(txt);
    }
UiText::UiText(const std::string& txt)
    : RectangleNode({0.0f, 0.0f, 0.0f, 0.0f}) {
        text_component.set_text(txt);
    }

const TextComponent& UiText::get_text_component() {
    return text_component;
}

std::string UiText::get_string() {
    return text_component.to_string();
}

void UiText::draw() {
    text_component.draw();
}


// Button
Button::Button(Rectangle r) : RectangleNode(r) {
    add_tag("Button");
}

void Button::set_texture(const Texture2D* t) {
    texture = t;
}

void Button::set_subject(ButtonState state, ButtonStateSubject* sub) {
    button_comp.set_subject(state, sub);
}

ButtonStateSubject* Button::get_subject(ButtonState state) {
    return button_comp.get_subject(state);
}

void Button::configure() {
    button_comp.configure();
}

// TODO: move state handling to component
void Button::update(float dt) {
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

std::string Button::to_string() {
    return fmt::format(
        "{} at {}: {}",
        get_tag(),
        get_world_pos(),
        button_comp.get_current_state()
    );
}

void Button::draw() {
    if (texture != nullptr) {
        // spdlog::info("{}", get_world_pos());
        DrawTextureV(*texture, get_world_pos(), WHITE);
    }
}

// TextButton (maybe remove it later)
TextButton::TextButton(Text txt)
    : Button({0.0f, 0.0f, 0.0f, 0.0f}) {
        text_component.set_text(txt);
    }
TextButton::TextButton(const std::string& txt)
    : Button({0.0f, 0.0f, 0.0f, 0.0f}) {
        text_component.set_text(txt);
    }

void TextButton::set_text(const std::string& _txt) {
    text_component.set_text(_txt);
}

void TextButton::draw() {
    text_component.draw();
}


// FrameCounter
FrameCounter::FrameCounter() : RectangleNode({0.0f, 0.0f, 0.0f, 0.0f}) {
    add_tag("FPS Counter");
}

void FrameCounter::update(float) {
    int new_fps = GetFPS();
    if (new_fps != fps_value) {
        fps_value = new_fps;
        text_component.set_text(TextFormat(format, fps_value));
    }
}

void FrameCounter::draw() {
    text_component.draw();
}


// MousePosReporter
MousePosReporter::MousePosReporter() : RectangleNode({0.0f, 30.0f, 0.0f, 0.0f}) {
    add_tag("MousePosReporter");
}

void MousePosReporter::update(float) {
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

void MousePosReporter::draw() {
    text_comp.draw();
}


// NodeInspector
NodeInspector::NodeInspector(LayerStorage* r)
    : RectangleNode({0.0f, 90.0f, 0.0f, 0.0f})
    , report_from(r) {
        add_tag("Node Inspector");
    }

void NodeInspector::update(float) {
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

void NodeInspector::draw() {
    text_comp.draw();
}
