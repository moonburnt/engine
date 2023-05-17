#include "engine/ui/components.hpp"

// Text comp
void TextComponent::set_text(Text txt) {
    text = txt;
    // For now, won't change parent's size coz it clashes with other
    // components. Maybe I will figure out the better solution for that l8r
    // TODO
    parent->set_size(text.get_size(font, size, spacing));
}

void TextComponent::set_text(const std::string& txt) {
    text.set_text(txt);
    // rect = text.get_rect();
    parent->set_size(text.get_size(font, size, spacing));
}

void TextComponent::set_size(int s) {
    size = s;
}

const Text& TextComponent::get_text() {
    return text;
}

void TextComponent::set_color(Color c) {
    color = c;
}

void TextComponent::set_spacing(int sp) {
    spacing = sp;
}

void TextComponent::set_font(Font f) {
    font = f;
}

std::string TextComponent::to_string() {
    return text.to_string();
}

void TextComponent::draw() {
    text.draw(parent->get_world_pos(), font, size, spacing, color);
}


// ButtonComponent
ButtonComponent::ButtonComponent(RectangleNode* p) : UiComponent(p) {}

void ButtonComponent::change_state(float dt, bool force) {
    if (future_state == std::nullopt) {
        return;
    }

    if (force || (future_state != current_state)) {
        current_state = future_state.value();
        if (bs_subjects.find(current_state) != bs_subjects.end()) {
            // Maybe use get_subject() instead of at()?
            bs_subjects.at(current_state)->set_changed();
            bs_subjects.at(current_state)->notify_observers(dt);
        }
    }
    future_state = std::nullopt;
}

void ButtonComponent::change_state(float dt) {
    change_state(dt, false);
}

void ButtonComponent::configure() {
    schedule_state_change(current_state);
    change_state(0.0f, true);
}

void ButtonComponent::set_subject(ButtonState state, ButtonStateSubject* sub) {
    bs_subjects[state] = sub;
}

ButtonStateSubject* ButtonComponent::get_subject(ButtonState state) {
    // default-initialize in case it does not exist
    if (bs_subjects.find(state) == bs_subjects.end()) {
        bs_subjects[state] = new ButtonStateSubject();
    }

    return bs_subjects.at(state);
}

// Schedule new state to be applied on next update() call
void ButtonComponent::schedule_state_change(ButtonState state) {
    future_state = state;
}

ButtonState ButtonComponent::get_current_state() {
    return current_state;
}

void ButtonComponent::update(float dt) {
    change_state(dt);
}
