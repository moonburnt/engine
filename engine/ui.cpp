#include "ui.hpp"

#include <algorithm>
#include <optional>
#include <raylib.h>
#include <string>
#include <stdexcept>
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
    DrawText(text.c_str(), real_pos.x, real_pos.y, text_size, text_color);
}

// Text Input Field
TextInputField::TextInputField(
    const std::string& default_text,
    Vector2 pos,
    size_t max_size,
    const char blink_char,
    float blink_frequency)
    : Label("", pos)
    , bg_text(default_text)
    , max_size(max_size)
    , blink_char({blink_char})
    , blink_timer(blink_frequency) {

    update_blink_pos();

    if (max_size != 0) {
        text.reserve(max_size);
    }

    blink_timer.start();
}

TextInputField::TextInputField(const std::string& default_text, Vector2 pos, size_t max_size)
    : TextInputField(default_text, pos, max_size, '_', 0.5f) {
}

TextInputField::TextInputField(const std::string& default_text, Vector2 pos)
    : TextInputField(default_text, pos, 0) {}

void TextInputField::update_blink_pos() {
    blink_pos = {
        real_pos.x + 2 + MeasureText(text.c_str(), text_size),
        real_pos.y + 4};
}

void TextInputField::set_text(const std::string& txt) {
    if (max_size != 0 && max_size < txt.length()) {
        // Doing it like that to avoid damaging original string
        std::string txt_copy = txt;
        txt_copy.resize(max_size);
        Label::set_text(txt_copy);
    }
    else {
        Label::set_text(txt);
    }
}

void TextInputField::set_pos(Vector2 pos, bool center) {
    Label::set_pos(pos, center);
    update_blink_pos();
}

void TextInputField::set_pos(Vector2 pos) {
    Label::set_pos(pos);
    update_blink_pos();
}

void TextInputField::enable() {
    if (is_enabled) {
        return;
    }

    is_enabled = true;
    blink_timer.start();
}

void TextInputField::disable() {
    if (!is_enabled) {
        return;
    }

    is_enabled = false;
    draw_blink = false;
}

bool TextInputField::is_empty() {
    return (text.length() == 0);
}

bool TextInputField::is_full() {
    return (max_size != 0 && text.length() == max_size);
}

void TextInputField::update(float dt) {
    if (!is_enabled) {
        return;
    }

    int key = GetCharPressed();

    bool text_changed = false;

    // Support for multiple characters pressed on the same frame
    while (key > 0) {
        // Only allow keys in between 32 to 125
        if ((key >= 32) && (key <= 125)) {
            if (max_size != 0 && text.length() >= max_size) {
                break;
            }

            text += static_cast<char>(key);
            text_changed = true;
        }

        // Switching key to next char in queue (if there is such char, else 0)
        key = GetCharPressed();
    }

    // TODO: add support for backspace being held, as right now this require
    // it to be pressed manually for each symbol
    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (text.length() > 0) {
            text.pop_back();
            text_changed = true;
        }
    }

    if (blink_timer.tick(dt)) {
        draw_blink = !draw_blink;
        blink_timer.start();
    }

    if (text_changed) {
        update_blink_pos();
    }
}

void TextInputField::draw() {
    if (text.length() > 0) {
        Label::draw();
    }
    else {
        DrawText(
            bg_text.c_str(),
            real_pos.x,
            real_pos.y,
            bg_text_size,
            bg_text_color);
    }

    if (draw_blink) {
        DrawText(
            blink_char.c_str(),
            blink_pos.x,
            blink_pos.y,
            text_size,
            text_color);
    }
}

// Buttons
void Button::reset_state() {
    state = ButtonStates::idle;
    last_state = ButtonStates::idle;
}

// Constructors don't need to specify return state
Button::Button(
    std::unordered_map<ButtonStates, const Texture2D*> textures,
    std::unordered_map<ButtonStates, const Sound*> sounds,
    Rectangle rectangle)
    : pos({0.0f, 0.0f})
    , state(ButtonStates::idle)
    , manual_update_mode(false)
    , textures(textures)
    , sounds(sounds)
    , rect(rectangle)
    , last_state(ButtonStates::idle) {
}

// TODO: constructor with option for text offset
Button::Button(
    const std::string& txt,
    std::unordered_map<ButtonStates, const Texture2D*> textures,
    std::unordered_map<ButtonStates, const Sound*> sounds,
    Rectangle rectangle)
    : Button(
          textures,
          sounds,
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

void Button::enable() {
    if (state != ButtonStates::disabled) {
        return;
    }
    else {
        state = ButtonStates::idle;
    }
}

void Button::disable() {
    if (state == ButtonStates::disabled) {
        return;
    }
    else {
        state = ButtonStates::disabled;
    }
}

bool Button::is_enabled() {
    return state != ButtonStates::disabled;
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
    if (manual_update_mode | (state == ButtonStates::disabled)) {
        return state;
    }

    if (CheckCollisionPointRec(GetMousePosition(), rect)) {
        if (last_state == ButtonStates::pressed) {
            if (IsMouseButtonUp(MOUSE_BUTTON_LEFT)) {
                state = ButtonStates::clicked;
                // if (on_click_callback != nullptr) {
                //     on_click_callback();
                // }
            }
        }
        else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            state = ButtonStates::pressed;
        }

        else {
            state = ButtonStates::hover;
        }
    }
    else {
        state = ButtonStates::idle;
    }

    if (state != last_state && sounds.count(state)) {
        PlaySound(*sounds[state]);
    }

    // Moved this there to avoid heap-use-after-free errors
    if (state == ButtonStates::clicked && on_click_callback != nullptr) {
        on_click_callback();
    }

    last_state = state;

    return state;
}

void Button::draw() {
    if (textures.count(state)) {
        DrawTexture(*textures[state], pos.x, pos.y, WHITE);
    }

    if (text != nullptr) {
        text->draw();
    }
}

void Button::set_state(ButtonStates _state) {
    // if (_state == ButtonStates::clicked) PlaySound(*sounds[1]);
    // else if (_state == ButtonStates::hover) PlaySound(*sounds[0]);

    state = _state;
    if (state != last_state && sounds.count(state)) {
        PlaySound(*sounds[state]);
    }
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
    std::unordered_map<ButtonStates, const Texture2D*> textures_on,
    std::unordered_map<ButtonStates, const Texture2D*> textures_off,
    std::unordered_map<ButtonStates, const Sound*> sounds,
    Rectangle rectangle,
    bool default_state)
    : Button(
          textures_on,
          sounds,
          rectangle)
    , textures_off(textures_off)
    , toggle_state(default_state)
    , state_switched(false) {
}

Checkbox::Checkbox(
    std::unordered_map<ButtonStates, const Texture2D*> textures_on,
    std::unordered_map<ButtonStates, const Texture2D*> textures_off,
    std::unordered_map<ButtonStates, const Sound*> sounds,
    Rectangle rectangle)
    : Checkbox(
        textures_on,
        textures_off,
        sounds,
        rectangle,
        true) {}

bool Checkbox::get_toggle() {
    return toggle_state;
}

void Checkbox::toggle(bool _toggle_state) {
    Button::reset_state();
    // if (toggle_state != _toggle_state) state_switched = true;
    toggle_state = _toggle_state;
}

void Checkbox::toggle() {
    if (toggle_state) {
        toggle(false);
    }
    else {
        toggle(true);
    }

    if (state_switched) {
        state_switched = false;
    }
    else {
        state_switched = true;
    }
}

void Checkbox::draw() {
    if (toggle_state) {
        Button::draw();
    }

    else if (textures_off.count(state)) {
        DrawTexture(*textures_off[state], pos.x, pos.y, WHITE);
    }
}

ButtonStates Checkbox::update() {
    Button::update();
    if (Button::is_clicked()) {
        toggle();
    }
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
    if (manual_update_mode) {
        button->set_manual_update_mode(true);
    }
}

void ButtonStorage::set_manual_update_mode(bool mode) {
    if (mode == manual_update_mode) {
        return;
    }

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
