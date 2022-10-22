#include "ui.hpp"

#include <algorithm>
#include <optional>
#include "raylib.h"
#include <string>
#include <stdexcept>
#include <tuple>
#include "raybuff.hpp"
#include "spdlog/spdlog.h"

// Widget
// void Widget::set_align(Align _align) {
//     align = _align;
//     // set_pos(get_pos());
// }

// Align Widget::get_align() {
//     return align;
// }

// void Widget::center() {
//     set_align(Align::Center);
// }

// // Label
// Label::Label(const std::string& txt, Vector2 position)
//     : text(txt)
//     , font(GetFontDefault()) // TODO: make it customizable
//     , real_pos(position) {
//     Node::set_pos(position);
// }

// void Label::set_pos(Vector2 _pos, bool apply_align) {
//    // This may be resource consuming and overkill
//     // TODO: consider reimagining the concept of aligns
//     // to use functions like get_topleft() from parent node to
//     // set align, instead of affecting current node's offset.
//     Node::set_pos(_pos);

//     // if (!auto_align) {
//     if (!auto_align || !apply_align) {
//         real_pos = pos;
//         return;
//     }

//     switch (align) {
//     case Align::TopLeft: {
//         real_pos = pos;
//         break;
//     }
//     case Align::Top: {
//         real_pos = {pos.x - MeasureText(text.c_str(), text_size) / 2, pos.y};
//         break;
//     }
//     case Align::TopRight: {
//         real_pos = {pos.x - MeasureText(text.c_str(), text_size), pos.y};
//         break;
//     }
//     case Align::Left: {
//         real_pos = {
//             pos.x,
//             pos.y - MeasureTextEx(
//                 font, text.c_str(), text_size, text_size / 10.0f).y / 2.0f};
//         break;
//     }
//     case Align::Center: {
//         Vector2 t_size = MeasureTextEx(
//                 font, text.c_str(), text_size, text_size / 10.0f);
//         // real_pos = {pos.x - text_size.x / 2, pos.y - text_size.y / 2};
//         real_pos = pos - t_size / 2.0f;
//         break;
//     }
//     case Align::Right: {
//         Vector2 t_size = MeasureTextEx(
//                 font, text.c_str(), text_size, text_size / 10.0f);
//         real_pos = {pos.x - t_size.x, pos.y - t_size.y / 2};
//         break;
//     }
//     case Align::BottomLeft: {
//         real_pos = {
//             pos.x,
//             pos.y - MeasureTextEx(
//                 font, text.c_str(), text_size, text_size / 10.0f).y};
//         break;
//     }
//     case Align::Bottom: {
//         Vector2 t_size = MeasureTextEx(
//                 font, text.c_str(), text_size, text_size / 10.0f);
//         real_pos = {pos.x - t_size.x / 2, pos.y - t_size.y};
//         break;
//     }
//     case Align::BottomRight: {
//         real_pos = pos - MeasureTextEx(
//                 font, text.c_str(), text_size, text_size / 10.0f);
//         // real_pos = {pos.x - text_size.x, pos.y - text_size.y};
//         break;
//     }
//     }
// }

// void Label::set_pos(Vector2 _pos) {
//     set_pos(_pos, auto_align);
// }

// Vector2 Label::get_pos() {
//     return real_pos;
// }

// void Label::apply_align() {
//     set_pos(Node::get_pos(), true);
// }

// void Label::set_text(const std::string& txt) {
//     text = txt;
//     if (auto_align) {
//         // set_pos(get_pos());
//         apply_align();
//     }
// }

// std::string Label::get_text() {
//     return text;
// }

// void Label::center() {
//     Widget::center();
//     apply_align();
// }

// void Label::draw() {
//     // TODO: rework this to use get_abs_pos()
//     // DrawText(text.c_str(), real_pos.x, real_pos.y, text_size, text_color);
//     DrawTextEx(font, text.c_str(), get_world_pos(), text_size, text_size/10, text_color);
// }

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

// Text Input Field
// TextInputField::TextInputField(
//     const std::string& default_text,
//     size_t max_size,
//     const char blink_char,
//     float blink_frequency)
//     : TextNode("")
//     , bg_text(default_text)
//     , max_size(max_size)
//     , blink_char({blink_char})
//     , blink_timer(blink_frequency) {

//     update_blink_pos();

//     if (max_size != 0) {
//         text.reserve(max_size);
//     }

//     blink_timer.start();
// }

// TextInputField::TextInputField(const std::string& default_text, size_t max_size)
//     : TextInputField(default_text, max_size, '_', 0.5f) {
// }

// TextInputField::TextInputField(const std::string& default_text)
//     : TextInputField(default_text, 0) {}

// void TextInputField::update_blink_pos() {
//     blink_pos =  get_abs_pos() + text.get_size() + {2.0f, 4.0f};
// }

// void TextInputField::set_text(const std::string& txt) {
//     if (max_size != 0 && max_size < txt.length()) {
//         // Doing it like that to avoid damaging original string
//         std::string txt_copy = txt;
//         txt_copy.resize(max_size);
//         Label::set_text(txt_copy);
//     }
//     else {
//         Label::set_text(txt);
//     }
// }

// void TextInputField::set_pos(Vector2 pos) {
//     Label::set_pos(pos);
//     update_blink_pos();
// }

// void TextInputField::enable() {
//     if (is_enabled) {
//         return;
//     }

//     is_enabled = true;
//     blink_timer.start();
// }

// void TextInputField::disable() {
//     if (!is_enabled) {
//         return;
//     }

//     is_enabled = false;
//     draw_blink = false;
// }

// bool TextInputField::is_empty() {
//     return (text.length() == 0);
// }

// bool TextInputField::is_full() {
//     return (max_size != 0 && text.length() == max_size);
// }

// void TextInputField::update(float dt) {
//     if (!is_enabled) {
//         return;
//     }

//     int key = GetCharPressed();

//     bool text_changed = false;

//     // Support for multiple characters pressed on the same frame
//     while (key > 0) {
//         // Only allow keys in between 32 to 125
//         if ((key >= 32) && (key <= 125)) {
//             if (max_size != 0 && text.length() >= max_size) {
//                 break;
//             }

//             text += static_cast<char>(key);
//             text_changed = true;
//         }

//         // Switching key to next char in queue (if there is such char, else 0)
//         key = GetCharPressed();
//     }

//     // TODO: add support for backspace being held, as right now this require
//     // it to be pressed manually for each symbol
//     if (IsKeyPressed(KEY_BACKSPACE)) {
//         if (text.length() > 0) {
//             text.pop_back();
//             text_changed = true;
//         }
//     }

//     if (blink_timer.tick(dt)) {
//         draw_blink = !draw_blink;
//         blink_timer.start();
//     }

//     if (text_changed) {
//         update_blink_pos();
//     }
// }

// void TextInputField::draw() {
//     if (text.length() > 0) {
//         Label::draw();
//     }
//     else {
//         DrawText(
//             bg_text.c_str(),
//             real_pos.x,
//             real_pos.y,
//             bg_text_size,
//             bg_text_color);
//     }

//     if (draw_blink) {
//         DrawText(
//             blink_char.c_str(),
//             blink_pos.x,
//             blink_pos.y,
//             text_size,
//             text_color);
//     }
// }

// // Buttons
// void Button::reset_state() {
//     state = ButtonStates::idle;
//     last_state = ButtonStates::idle;
// }

// // Constructors don't need to specify return state
// Button::Button(
//     std::unordered_map<ButtonStates, const Texture2D*> textures,
//     std::unordered_map<ButtonStates, const Sound*> sounds,
//     Rectangle rectangle)
//     : pos({0.0f, 0.0f})
//     , state(ButtonStates::idle)
//     , manual_update_mode(false)
//     , textures(textures)
//     , sounds(sounds)
//     , rect(rectangle)
//     , last_state(ButtonStates::idle) {
// }

// // TODO: constructor with option for text offset
// Button::Button(
//     const std::string& txt,
//     std::unordered_map<ButtonStates, const Texture2D*> textures,
//     std::unordered_map<ButtonStates, const Sound*> sounds,
//     Rectangle rectangle)
//     : Button(
//           textures,
//           sounds,
//           rectangle) {
//     // I'm not sure if this should be based on center of rect or on center of
//     // texture. For now it's done like that, may change in future.
//     text = new Label(txt, center_text(txt, {rect.width / 2.0f, rect.height / 2.0f}));
// }

// Button::~Button() {
//     if (text != nullptr) {
//         delete text;
//     }
// }

// void Button::enable() {
//     if (state != ButtonStates::disabled) {
//         return;
//     }
//     else {
//         state = ButtonStates::idle;
//     }
// }

// void Button::disable() {
//     if (state == ButtonStates::disabled) {
//         return;
//     }
//     else {
//         state = ButtonStates::disabled;
//     }
// }

// bool Button::is_enabled() {
//     return state != ButtonStates::disabled;
// }

// void Button::set_callback(std::function<void()> _on_click_callback) {
//     on_click_callback = _on_click_callback;
// }

// void Button::set_text(const std::string& txt) {
//     if (text == nullptr) {
//         text = new Label(txt, center_text(txt, {rect.width / 2.0f, rect.height / 2.0f}));
//     }
//     else {
//         text->set_text(txt);
//         text->center();
//     }
// }

// enum ButtonStates Button::update() {
//     if (manual_update_mode | (state == ButtonStates::disabled)) {
//         return state;
//     }

//     if (CheckCollisionPointRec(GetMousePosition(), rect)) {
//         if (last_state == ButtonStates::pressed) {
//             if (IsMouseButtonUp(MOUSE_BUTTON_LEFT)) {
//                 state = ButtonStates::clicked;
//                 // if (on_click_callback != nullptr) {
//                 //     on_click_callback();
//                 // }
//             }
//         }
//         else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
//             state = ButtonStates::pressed;
//         }

//         else {
//             state = ButtonStates::hover;
//         }
//     }
//     else {
//         state = ButtonStates::idle;
//     }

//     if (state != last_state && sounds.count(state)) {
//         PlaySound(*sounds[state]);
//     }

//     // Moved this there to avoid heap-use-after-free errors
//     if (state == ButtonStates::clicked && on_click_callback != nullptr) {
//         on_click_callback();
//     }

//     last_state = state;

//     return state;
// }

// void Button::draw() {
//     if (textures.count(state)) {
//         DrawTexture(*textures[state], pos.x, pos.y, WHITE);
//     }

//     if (text != nullptr) {
//         text->draw();
//     }
// }

// void Button::set_state(ButtonStates _state) {
//     // if (_state == ButtonStates::clicked) PlaySound(*sounds[1]);
//     // else if (_state == ButtonStates::hover) PlaySound(*sounds[0]);

//     state = _state;
//     if (state != last_state && sounds.count(state)) {
//         PlaySound(*sounds[state]);
//     }
//     last_state = state;
// }

// ButtonStates Button::get_state() {
//     return state;
// }

// void Button::set_manual_update_mode(bool mode) {
//     manual_update_mode = mode;
// }

// void Button::set_pos(Vector2 position) {
//     // Hitbox should always retain initial position diff
//     // TODO: maybe save diff as class-wide variable?

//     int x_diff = rect.x - pos.x;
//     int y_diff = rect.y - pos.y;

//     pos = position;

//     rect.x = position.x + x_diff;
//     rect.y = position.y + y_diff;

//     if (text != nullptr) {
//         text->set_pos(
//             {position.x + (rect.width / 2.0f), position.y + (rect.height / 2.0f)});
//         text->center();
//     }
// }

// void Button::center() {
//     // It works a bit differently than Label's, coz I did not need real_pos yet
//     // It uses pos and not rect's values coz rect's x, y are used as offsets
//     // set_pos(get_rect_center({pos.x, pos.y, rect.width, rect.height}));
//     set_pos({pos.x - rect.width/2.0f, pos.y - rect.height / 2.0f});
// }

// Vector2 Button::get_pos() {
//     return pos;
// }

// Rectangle Button::get_rect() {
//     return rect;
// }

// // Getter for private clicked var. This allows to make it readable, but prevent
// // overwriting from outside
// bool Button::is_clicked() {
//     return state == ButtonStates::clicked;
// }

// // Checkbox shenanigans
// Checkbox::Checkbox(
//     std::unordered_map<ButtonStates, const Texture2D*> textures_on,
//     std::unordered_map<ButtonStates, const Texture2D*> textures_off,
//     std::unordered_map<ButtonStates, const Sound*> sounds,
//     Rectangle rectangle,
//     bool default_state)
//     : Button(
//           textures_on,
//           sounds,
//           rectangle)
//     , textures_off(textures_off)
//     , toggle_state(default_state)
//     , state_switched(false) {
// }

// Checkbox::Checkbox(
//     std::unordered_map<ButtonStates, const Texture2D*> textures_on,
//     std::unordered_map<ButtonStates, const Texture2D*> textures_off,
//     std::unordered_map<ButtonStates, const Sound*> sounds,
//     Rectangle rectangle)
//     : Checkbox(
//         textures_on,
//         textures_off,
//         sounds,
//         rectangle,
//         true) {}

// bool Checkbox::get_toggle() {
//     return toggle_state;
// }

// void Checkbox::toggle(bool _toggle_state) {
//     Button::reset_state();
//     // if (toggle_state != _toggle_state) state_switched = true;
//     toggle_state = _toggle_state;
// }

// void Checkbox::toggle() {
//     if (toggle_state) {
//         toggle(false);
//     }
//     else {
//         toggle(true);
//     }

//     if (state_switched) {
//         state_switched = false;
//     }
//     else {
//         state_switched = true;
//     }
// }

// void Checkbox::draw() {
//     if (toggle_state) {
//         Button::draw();
//     }

//     else if (textures_off.count(state)) {
//         DrawTexture(*textures_off[state], pos.x, pos.y, WHITE);
//     }
// }

// ButtonStates Checkbox::update() {
//     Button::update();
//     if (Button::is_clicked()) {
//         toggle();
//     }
//     return state;
// }

// bool Checkbox::is_clicked() {
//     return state_switched;
// }

// void Checkbox::reset_state() {
//     Button::reset_state();
//     state_switched = false;
// }

// // Button Storage
// ButtonStorage::ButtonStorage() : manual_update_mode(false) {}

// ButtonStorage::~ButtonStorage() {
//     // This may be not the right way to do things, idk
//     for (auto i : storage) {
//         delete i;
//     }
// }

// ButtonBase* ButtonStorage::operator[](int i) {
//     return storage[i];
// }

// ButtonBase* ButtonStorage::at(int i) {
//     return storage.at(i);
// }

// void ButtonStorage::add_button(ButtonBase* button) {
//     storage.push_back(button);
//     if (manual_update_mode) {
//         button->set_manual_update_mode(true);
//     }
// }

// void ButtonStorage::set_manual_update_mode(bool mode) {
//     if (mode == manual_update_mode) {
//         return;
//     }

//     if (mode) {
//         for (auto i : storage) {
//             i->set_manual_update_mode(true);
//             i->reset_state();
//         }
//     }
//     else {
//         for (auto i : storage) {
//             i->set_manual_update_mode(false);
//             i->reset_state();
//         }
//     }

//     manual_update_mode = mode;
// }

// void ButtonStorage::select_button(size_t button) {
//     if (!storage.empty() && button < storage.size()) {
//         set_manual_update_mode(true);
//         if (selected_button != std::nullopt) {
//             storage[selected_button.value()]->reset_state();
//         }
//         selected_button = button;
//         storage[selected_button.value()]->set_state(ButtonStates::hover);
//     }
// }

// void ButtonStorage::select_next(bool cycle) {
//     int button;
//     if (cycle && (selected_button.value() + 1ul == storage.size())) {
//         button = 0;
//     }
//     else {
//         button = selected_button.value() + 1;
//     }

//     select_button(button);
// }

// void ButtonStorage::select_previous(bool cycle) {
//     int button;
//     if (cycle && (static_cast<int>(selected_button.value()) - 1 < 0)) {
//         // TODO: may need to set it to size-1.
//         button = storage.size();
//     }
//     else {
//         button = selected_button.value() - 1;
//     }

//     select_button(button);
// }

// void ButtonStorage::update() {
//     if (manual_update_mode) {
//         if (selected_button != std::nullopt) {
//             storage[selected_button.value()]->update();
//         }
//     }
//     else {
//         for (auto button : storage) {
//             // This may be a bad idea in long run, but for now it should protect
//             // from segfaults caused by attempting to update something if callback
//             // attached to button's update has told storage's instance to die.
//             if (button->update() == ButtonStates::clicked) {
//                 return;
//             };
//         }
//     }
// }

// std::optional<std::tuple<int, ButtonStates>> ButtonStorage::get_selected_button_state() {
//     if (selected_button != std::nullopt) {
//         return std::make_tuple(
//             selected_button.value(), storage[selected_button.value()]->get_state());
//     }

//     return std::nullopt;
// }

// bool ButtonStorage::set_selected_button_state(ButtonStates state) {
//     if (selected_button == std::nullopt) {
//         return false;
//     }

//     storage[selected_button.value()]->set_state(state);

//     return true;
// }

// void ButtonStorage::draw() {
//     for (auto button : storage) {
//         button->draw();
//     }
// }

// // Vertical Container

// VerticalContainer::VerticalContainer(float _gap)
//     : gap(_gap) {
// }

// void VerticalContainer::set_pos(Vector2 position) {
//     float x_diff = rect.x - pos.x;
//     float y_diff = rect.y - pos.y;

//     pos = position;

//     rect.x = position.x + x_diff;
//     rect.y = position.y + y_diff;

//     if (storage.size() > 0) {
//         Vector2 first_pos = storage.front()->get_pos();
//         float btn_x = first_pos.x + (position.x - first_pos.x);
//         float btn_y = first_pos.y + (position.y - first_pos.y);

//         for (auto btn : storage) {
//             // This may be jank if buttons have different x. Won't care for now.
//             // TODO
//             Rectangle current_rect = btn->get_rect();

//             btn->set_pos({btn_x, btn_y});

//             btn_y += current_rect.height + gap;
//         }
//     }
// }

// void VerticalContainer::center() {
//     set_pos({pos.x - rect.width / 2.0f, pos.y - rect.height / 2.0f});
// }

// void VerticalContainer::add_button(ButtonBase* button) {
//     // TODO: implement different button aligns. Right now these are mess

//     Vector2 new_pos;
//     Rectangle button_rect = button->get_rect();

//     if (storage.size() > 0) {
//         Rectangle last_rect = storage.back()->get_rect();

//         new_pos = {last_rect.x, last_rect.y + last_rect.height + gap};

//         rect.height += gap + button_rect.height;
//         rect.width = std::max(rect.width, button_rect.width);
//     }
//     else {
//         new_pos = pos;
//         rect = {pos.x, pos.y, button_rect.width, button_rect.height};
//     }

//     button->set_pos(new_pos);

//     ButtonStorage::add_button(button);
// }

// // Progress bar
// ProgressBar::ProgressBar(
//     Rectangle _rect,
//     Vector2 _pos,
//     const Texture2D* _bg_texture,
//     const Texture2D* _fg_texture)
//     : bg_texture(_bg_texture)
//     , fg_texture(_fg_texture)
//     , rect(_rect)
//     , width_percent(rect.width / (progress * 100.0f)) {
//     // Doing it like that, coz for right now default rect's pos is used with
//     // default-initialized pb's pos to calculate rect's offset
//     set_pos(_pos);
// }

// ProgressBar::ProgressBar(Rectangle _rect, Vector2 _pos)
//     : ProgressBar(_rect, _pos, nullptr, nullptr) {}

// ProgressBar::~ProgressBar() {
//     if (text != nullptr) {
//         delete text;
//     }
// }

// // void ProgressBar::set_bg_texture(const Texture2D* texture) {
// //     bg_texture = texture;
// // }

// // void ProgressBar::set_fg_texture(const Texture2D* texture) {
// //     fg_texture = texture;
// // }

// void ProgressBar::set_color(Color _color) {
//     color = _color;
// }

// void ProgressBar::set_pos(Vector2 _pos) {
//     rect.x = _pos.x + (rect.x - pos.x);
//     rect.y = _pos.y + (rect.y - pos.y);

//     pos = _pos;

//     if (text != nullptr) {
//         text->set_pos({pos.x + width_percent * 50, pos.y + rect.height / 2.0f});
//         text->center();
//     }
// }

// void ProgressBar::set_min_progress(float _min_progress) {
//     min_progress = _min_progress;
// }

// void ProgressBar::set_max_progress(float _max_progress) {
//     max_progress = _max_progress;
// }

// float ProgressBar::get_progress() {
//     return progress;
// }

// void ProgressBar::set_progress(float _progress) {
//     progress = std::clamp(_progress, min_progress, max_progress);
//     rect.width = width_percent * (progress * 100);
// }

// void ProgressBar::set_text(const std::string& _text) {
//     if (text != nullptr) {
//         text->set_text(_text);
//     }
//     else {
//         text = new Label(_text, get_rect_center(rect));
//     }
//     text->center();
// }

// void ProgressBar::draw() {
//     if (bg_texture != nullptr) {
//         DrawTexture(*bg_texture, pos.x, pos.y, WHITE);
//     }

//     DrawRectangle(
//         rect.x, rect.y, rect.width, rect.height, color);

//     if (fg_texture != nullptr) {
//         DrawTexture(*fg_texture, pos.x, pos.y, WHITE);
//     }

//     if (text != nullptr) {
//         text->draw();
//     }
// }
