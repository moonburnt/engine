#include "ui.hpp"
#include "utility.hpp"

#include <algorithm>
#include <optional>
#include <raylib.h>
#include <string>
#include <tuple>

// Label
Label::Label(const std::string& txt, Vector2 position)
    : text(txt)
    , pos(position)
    , real_pos(position) {
}

void Label::center() {
    real_pos = center_text(text, pos);
}

void Label::set_pos(Vector2 _pos, bool _center) {
    pos = _pos;
    if (_center) {
        center();
    }
    else {
        real_pos = pos;
    }
}

void Label::set_pos(Vector2 _pos) {
    set_pos(_pos, false);
}

Vector2 Label::get_pos() {
    return pos;
}

void Label::set_text(const std::string& txt) {
    text = txt;
}

void Label::draw() {
    DrawText(text.c_str(), real_pos.x, real_pos.y, DEFAULT_TEXT_SIZE, DEFAULT_TEXT_COLOR);
}

void Button::reset_state() {
    state = ButtonStates::idle;
    last_state = ButtonStates::idle;
}

// Constructors don't need to specify return state
Button::Button(
    const Texture2D* texture_default,
    const Texture2D* texture_hover,
    const Texture2D* texture_pressed,
    const Sound* sfx_hover,
    const Sound* sfx_click,
    Rectangle rectangle)
    : pos({0.0f, 0.0f})
    , state(ButtonStates::idle)
    , manual_update_mode(false)
    , rect(rectangle)
    , last_state(ButtonStates::idle) {
    textures[ButtonStates::idle] = texture_default;
    textures[ButtonStates::hover] = texture_hover;
    textures[ButtonStates::pressed] = texture_pressed;
    textures[ButtonStates::clicked] = texture_default;
    sounds[0] = sfx_hover;
    sounds[1] = sfx_click;
}

// TODO: constructor with option for text offset
Button::Button(
    const std::string& txt,
    const Texture2D* texture_default,
    const Texture2D* texture_hover,
    const Texture2D* texture_pressed,
    const Sound* sfx_hover,
    const Sound* sfx_click,
    Rectangle rectangle)
    : Button(
          texture_default,
          texture_hover,
          texture_pressed,
          sfx_hover,
          sfx_click,
          rectangle) {
    // I'm not sure if this should be based on center of rect or on center of
    // texture. For now it's done like that, may change in future.
    text = new Label(txt, center_text(txt, {rect.width / 2.0f, rect.height / 2.0f}));
}

Button::~Button() {
    if (text != nullptr) {
        delete text;
    }
}

void Button::set_callback(std::function<void()> _on_click_callback) {
    on_click_callback = _on_click_callback;
}

void Button::set_text(const std::string& txt) {
    if (text == nullptr) {
        text = new Label(txt, center_text(txt, {rect.width / 2.0f, rect.height / 2.0f}));
    }
    else {
        text->set_text(txt);
        text->center();
    }
}

enum ButtonStates Button::update() {
    if (manual_update_mode) return state;

    if (CheckCollisionPointRec(GetMousePosition(), rect)) {
        if (last_state == ButtonStates::pressed) {
            if (IsMouseButtonUp(MOUSE_BUTTON_LEFT)) {
                state = ButtonStates::clicked;
                PlaySound(*sounds[1]);
                if (on_click_callback != nullptr) {
                    on_click_callback();
                }
            }
        }
        else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            state = ButtonStates::pressed;
        }

        else {
            if (last_state != ButtonStates::hover) {
                PlaySound(*sounds[0]);
            }
            state = ButtonStates::hover;
        }
    }
    else {
        state = ButtonStates::idle;
    }

    last_state = state;

    return state;
}

void Button::draw() {
    DrawTexture(*textures[state], pos.x, pos.y, WHITE);
    if (text != nullptr) {
        text->draw();
    }
}

void Button::set_state(ButtonStates _state) {
    if (_state == ButtonStates::clicked) PlaySound(*sounds[1]);
    else if (_state == ButtonStates::hover) PlaySound(*sounds[0]);

    state = _state;
    last_state = state;
}

ButtonStates Button::get_state() {
    return state;
}

void Button::set_manual_update_mode(bool mode) {
    manual_update_mode = mode;
}

void Button::set_pos(Vector2 position) {
    // Hitbox should always retain initial position diff

    int x_diff = rect.x - pos.x;
    int y_diff = rect.y - pos.y;

    pos = position;

    rect.x = position.x + x_diff;
    rect.y = position.y + y_diff;

    if (text != nullptr) {
        text->set_pos(
            {position.x + (rect.width / 2.0f), position.y + (rect.height / 2.0f)});
        text->center();
    }
}

Vector2 Button::get_pos() {
    return pos;
}

Rectangle Button::get_rect() {
    return rect;
}

// Getter for private clicked var. This allows to make it readable, but prevent
// overwriting from outside
bool Button::is_clicked() {
    return state == ButtonStates::clicked;
}

// Checkbox shenanigans
Checkbox::Checkbox(
    const Texture2D* texture_on_default,
    const Texture2D* texture_on_hover,
    const Texture2D* texture_on_pressed,
    const Texture2D* texture_off_default,
    const Texture2D* texture_off_hover,
    const Texture2D* texture_off_pressed,
    const Sound* sfx_hover,
    const Sound* sfx_click,
    Rectangle rectangle,
    bool default_state)
    : Button(
          texture_on_default,
          texture_on_hover,
          texture_on_pressed,
          sfx_hover,
          sfx_click,
          rectangle) {
    textures_off[ButtonStates::idle] = texture_off_default,
    textures_off[ButtonStates::hover] = texture_off_hover,
    textures_off[ButtonStates::pressed] = texture_off_pressed,
    textures_off[ButtonStates::clicked] = texture_off_default,
    toggle_state = default_state;
    state_switched = false;
}

Checkbox::Checkbox(
    const Texture2D* texture_on_default,
    const Texture2D* texture_on_hover,
    const Texture2D* texture_on_pressed,
    const Texture2D* texture_off_default,
    const Texture2D* texture_off_hover,
    const Texture2D* texture_off_pressed,
    const Sound* sfx_hover,
    const Sound* sfx_click,
    Rectangle rectangle)
    : Checkbox(
          texture_on_default,
          texture_on_hover,
          texture_on_pressed,
          texture_off_default,
          texture_off_hover,
          texture_off_pressed,
          sfx_hover,
          sfx_click,
          rectangle,
          true) {
}

bool Checkbox::get_toggle() {
    return toggle_state;
}

void Checkbox::toggle(bool _toggle_state) {
    Button::reset_state();
    // if (toggle_state != _toggle_state) state_switched = true;
    toggle_state = _toggle_state;
}

void Checkbox::toggle() {
    if (toggle_state) toggle(false);
    else toggle(true);
    if (state_switched) state_switched = false;
    else state_switched = true;
}

void Checkbox::draw() {
    if (toggle_state) Button::draw();
    else DrawTexture(*textures_off[state], pos.x, pos.y, WHITE);
}

ButtonStates Checkbox::update() {
    Button::update();
    if (Button::is_clicked()) toggle();
    return state;
}

bool Checkbox::is_clicked() {
    return state_switched;
}

void Checkbox::reset_state() {
    Button::reset_state();
    state_switched = false;
}

// Button Storage
ButtonStorage::ButtonStorage() : manual_update_mode(false) {}

ButtonStorage::~ButtonStorage() {
    // This may be not the right way to do things, idk
    for (auto i : storage) {
        delete i;
    }
}

ButtonBase* ButtonStorage::operator[](int i) {
    return storage[i];
}

ButtonBase* ButtonStorage::at(int i) {
    return storage.at(i);
}

void ButtonStorage::add_button(ButtonBase* button) {
    storage.push_back(button);
    if (manual_update_mode) button->set_manual_update_mode(true);
}

void ButtonStorage::set_manual_update_mode(bool mode) {
    if (mode == manual_update_mode) return;

    if (mode) {
        for (auto i : storage) {
            i->set_manual_update_mode(true);
            i->reset_state();
        }
    }
    else {
        for (auto i : storage) {
            i->set_manual_update_mode(false);
            i->reset_state();
        }
    }

    manual_update_mode = mode;
}

void ButtonStorage::select_button(size_t button) {
    if (!storage.empty() && button < storage.size()) {
        set_manual_update_mode(true);
        if (selected_button != std::nullopt) {
            storage[selected_button.value()]->reset_state();
        }
        selected_button = button;
        storage[selected_button.value()]->set_state(ButtonStates::hover);
    }
}

void ButtonStorage::select_next(bool cycle) {
    int button;
    if (cycle && (selected_button.value() + 1ul == storage.size())) {
        button = 0;
    }
    else {
        button = selected_button.value() + 1;
    }

    select_button(button);
}

void ButtonStorage::select_previous(bool cycle) {
    int button;
    if (cycle && (static_cast<int>(selected_button.value()) - 1 < 0)) {
        // TODO: may need to set it to size-1.
        button = storage.size();
    }
    else {
        button = selected_button.value() - 1;
    }

    select_button(button);
}

void ButtonStorage::update() {
    if (manual_update_mode) {
        if (selected_button != std::nullopt) {
            storage[selected_button.value()]->update();
        }
    }
    else {
        for (auto button : storage) {
            // This may be a bad idea in long run, but for now it should protect
            // from segfaults caused by attempting to update something if callback
            // attached to button's update has told storage's instance to die.
            if (button->update() == ButtonStates::clicked) {
                return;
            };
        }
    }
}

std::optional<std::tuple<int, ButtonStates>> ButtonStorage::get_selected_button_state() {
    if (selected_button != std::nullopt) {
        return std::make_tuple(
            selected_button.value(), storage[selected_button.value()]->get_state());
    }

    return std::nullopt;
}

bool ButtonStorage::set_selected_button_state(ButtonStates state) {
    if (selected_button == std::nullopt) {
        return false;
    }

    storage[selected_button.value()]->set_state(state);

    return true;
}

void ButtonStorage::draw() {
    for (auto button : storage) {
        button->draw();
    }
}

// Vertical Container

VerticalContainer::VerticalContainer(float _gap)
    : gap(_gap) {
}

void VerticalContainer::set_pos(Vector2 position) {
    float x_diff = rect.x - pos.x;
    float y_diff = rect.y - pos.y;

    pos = position;

    rect.x = position.x + x_diff;
    rect.y = position.y + y_diff;

    if (storage.size() > 0) {
        Vector2 first_pos = storage.front()->get_pos();
        float btn_x = first_pos.x + (position.x - first_pos.x);
        float btn_y = first_pos.y + (position.y - first_pos.y);

        for (auto btn : storage) {
            // This may be jank if buttons have different x. Won't care for now.
            // TODO
            Rectangle current_rect = btn->get_rect();

            btn->set_pos({btn_x, btn_y});

            btn_y += current_rect.height + gap;
        }
    }
}

void VerticalContainer::center() {
    set_pos({pos.x - rect.width / 2.0f, pos.y - rect.height / 2.0f});
}

void VerticalContainer::add_button(ButtonBase* button) {
    // TODO: implement different button aligns. Right now these are mess

    Vector2 new_pos;
    Rectangle button_rect = button->get_rect();

    if (storage.size() > 0) {
        Rectangle last_rect = storage.back()->get_rect();

        new_pos = {last_rect.x, last_rect.y + last_rect.height + gap};

        rect.height += gap + button_rect.height;
        rect.width = std::max(rect.width, button_rect.width);
    }
    else {
        new_pos = pos;
        rect = {pos.x, pos.y, button_rect.width, button_rect.height};
    }

    button->set_pos(new_pos);

    ButtonStorage::add_button(button);
}
