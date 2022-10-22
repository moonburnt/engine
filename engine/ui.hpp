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
#include "input.hpp"

// UI primitives.
// TODO: change naming to make things start with Capital Character
enum class ButtonStates {
    idle,
    hover,
    pressed,
    clicked,
    disabled
};

// DEPRECATED, TODO: REMOVE OR REIMPLEMENT THESE
// class ButtonBase {
// public:
//     virtual ButtonStates update() = 0;
//     virtual void draw() = 0;
//     virtual void set_manual_update_mode(bool mode) = 0;
//     virtual void reset_state() = 0;
//     virtual void set_state(ButtonStates state) = 0;
//     virtual void set_pos(Vector2 position) = 0;
//     virtual ButtonStates get_state() = 0;
//     virtual Rectangle get_rect() = 0;
//     virtual bool is_clicked() = 0;
//     virtual Vector2 get_pos() = 0;

//     virtual ~ButtonBase() = default;
// };

// class Widget : public Node {
// protected:
//     Align align = Align::TopLeft;
//     // // Pos to draw things on. Different if align is not TopLeft
//     // TODO, for now only implemented in Label
//     // Vector2 real_pos;

// public:
//     virtual void set_align(Align _align) override;
//     Align get_align();

//     virtual void center();
// };

// // Basic label.
// class Label : public Widget {
// protected:
//     std::string text;
//     int text_size = DEFAULT_TEXT_SIZE;
//     Color text_color = DEFAULT_TEXT_COLOR;
//     Font font;
//     // Pos to draw things on. Different if align is not TopLeft
//     // TODO: move this to Widget
//     Vector2 real_pos;
//     // Auto align. If set to true - will auto align object on text change
//     bool auto_align = false;

//     void set_pos(Vector2 pos, bool apply_align);

// public:
//     Label(const std::string& txt, Vector2 position);

//     void set_pos(Vector2 pos) override;
//     // Returns real_pos to make get_abs_pos() work, idk if it makes sense. TODO
//     Vector2 get_pos() override;

//     // Apply current align
//     void apply_align();

//     // Set label's text. May need to re-center message after that.
//     void set_text(const std::string& txt);
//     // Unsure if I should return const std::string& there. TODO
//     std::string get_text();

//     void center() override;

//     void draw() override;
// };
// END OF DEPRECATION WARNING

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

// Simple text input field, for basic text editing.
class TextInputField : public BasicTextNode {
protected:
    AbstractDataProcessor<std::string, std::pair<std::string, bool>>* input_hander;
public:
    TextInputField(AbstractDataProcessor<std::string, std::pair<std::string, bool>>* _input_handler)
        : BasicTextNode("")
        , input_hander(_input_handler) {}

    void update(float dt) override {
        text.set_text(input_handler(text.get_text()));
    };
};

// class Button : public ButtonBase {
// protected:
//     // Button position. Hitbox will always appear at center of it
//     Vector2 pos;
//     // State is current button state
//     ButtonStates state;
//     // State update mode. See set_manual_update_mode();
//     bool manual_update_mode;
//     std::unordered_map<ButtonStates, const Texture2D*> textures;

//     // Callback function to be triggered if button has been clicked
//     std::function<void()> on_click_callback = nullptr;

//     Label* text = nullptr;

// private:
//     std::unordered_map<ButtonStates, const Sound*> sounds;
//     // Button's hitbox. X and Y are offsets from texture's top left
//     Rectangle rect;
//     // Button state from previous screen
//     ButtonStates last_state;

// public:
//     Button(
//         std::unordered_map<ButtonStates, const Texture2D*> textures,
//         std::unordered_map<ButtonStates, const Sound*> sounds,
//         Rectangle rectangle);

//     Button(
//         const std::string& text,
//         std::unordered_map<ButtonStates, const Texture2D*> textures,
//         std::unordered_map<ButtonStates, const Sound*> sounds,
//         Rectangle rectangle);

//     ~Button();

//     void enable();
//     void disable();
//     bool is_enabled();

//     // Set on_click_callback function
//     void set_callback(std::function<void()> on_click_callback);

//     void set_text(const std::string& txt);

//     ButtonStates update() override;
//     void draw() override;

//     // Set manual update mode. If true - won't update state with update(),
//     // instead will only respond to manual set_state() calls.
//     void set_manual_update_mode(bool mode) override;
//     // Override button state. Will also toggle on manual update mode.
//     void set_state(ButtonStates state) override;
//     ButtonStates get_state() override;
//     // Reset button state and last_state to ButtonStates::idle.
//     void reset_state() override;
//     void set_pos(Vector2 position) override;

//     void center();

//     bool is_clicked() override;
//     // This getter is necessary to use pos from outside without being able
//     // to overwrite its value directly (coz its meant to be done together
//     // with changing position of other button's elements)
//     Vector2 get_pos() override;
//     // Same there
//     Rectangle get_rect() override;
// };

// class Checkbox : public Button {
// private:
//     // Storage for textures of disabled checkbox
//     std::unordered_map<ButtonStates, const Texture2D*> textures_off;
//     // State of checkbox (on or off)
//     bool toggle_state;
//     // Has checkbox'es state been changed with toggle() or not.
//     bool state_switched;

// public:
//     Checkbox(
//         std::unordered_map<ButtonStates, const Texture2D*> textures_on,
//         std::unordered_map<ButtonStates, const Texture2D*> textures_off,
//         std::unordered_map<ButtonStates, const Sound*> sounds,
//         Rectangle rectangle);

//     Checkbox(
//         std::unordered_map<ButtonStates, const Texture2D*> textures_on,
//         std::unordered_map<ButtonStates, const Texture2D*> textures_off,
//         std::unordered_map<ButtonStates, const Sound*> sounds,
//         Rectangle rectangle,
//         bool default_state);

//     // Returns toggle state of checkbox
//     bool get_toggle();
//     // Toggle checkbox on or off. Doesn't affect "state_switched".
//     void toggle(bool toggle_state);
//     // Toggle checkbox state back and forth. Toggles "state_switched", until
//     // reset_state() is used.
//     void toggle();

//     void draw() override;
//     ButtonStates update() override;

//     // Returns state_switched.
//     bool is_clicked() override;
//     // Resets state_switched.
//     void reset_state() override;
// };

// class ButtonStorage {
// protected:
//     // Storage that holds buttons. Since there is no way to remove these, its
//     // a vector. May rework into map if there will be need to remove elements.
//     std::vector<ButtonBase*> storage;
//     // Number of selected button in storage
//     // It may be an overkill to use optional there, but otherwise there would be
//     // issues with setting unsettable default that will compile.
//     std::optional<size_t> selected_button;
//     bool manual_update_mode;

// public:
//     ButtonStorage();
//     ~ButtonStorage();
//     // Add button or its derivatives to storage.
//     virtual void add_button(ButtonBase* button);
//     void set_manual_update_mode(bool mode);
//     // Select button with specific id in storage. Any of these selectors will
//     // automatically toggle on set_manual_update_mode().
//     void select_button(size_t button);
//     void select_next(bool cycle);
//     void select_previous(bool cycle);
//     // Update all buttons assigned to storage. If manual_update_mode has been set
//     // to true - only updates selected button.
//     void update();
//     // Set state of selected button. Returns true if there is such button, else false
//     bool set_selected_button_state(ButtonStates state);
//     // Draw all buttons in storage
//     void draw();

//     std::optional<std::tuple<int, ButtonStates>> get_selected_button_state();

//     // Overload to [], that provides access to storage elements.
//     // For now, doesn't have safety checks - requiesting out of bounds value will
//     // probably lead to segfault.
//     ButtonBase* operator[](int i);
//     ButtonBase* at(int i);
// };

// class VerticalContainer : public ButtonStorage {
// private:
//     // Container position. Rect will always appear at center of it
//     Vector2 pos = {0.0f, 0.0f};

//     // Container's own rectangle. Its size change dynamically on addition of
//     // new items. Width is equal to width of widest element and height - height
//     // of all elements total.
//     Rectangle rect = {0.0f, 0.0f, 0.0f, 0.0f};

//     // Gap between items
//     float gap;

// public:
//     VerticalContainer(float gap);

//     void set_pos(Vector2 pos);

//     void center();

//     void add_button(ButtonBase* button) override;
// };

// class ProgressBar {
// protected:
//     const Texture2D* bg_texture = nullptr;
//     Color color = GREEN;
//     const Texture2D* fg_texture = nullptr;
//     Label* text = nullptr;
//     Rectangle rect;
//     Vector2 pos = {0.0f, 0.0f};
//     // Progress limits. 0.0f for 0% and 1.0f for 100%
//     float min_progress = 0.0f;
//     float max_progress = 1.0f;
//     // Current progress. Must be between limits above
//     float progress = 1.0f;
//     // 1% of rect width, used to set its new width.
//     float width_percent;

// public:
//     // TODO: add rect pos offset coz rn its calculated weirdly
//     ProgressBar(
//         Rectangle _rect,
//         Vector2 _pos,
//         const Texture2D* _bg_texture,
//         const Texture2D* _fg_texture);
//     ProgressBar(Rectangle _rect, Vector2 _pos);
//     ~ProgressBar();

//     // void set_bg_texture(const Texture2D* texture);
//     // void set_fg_texture(const Texture2D* texture);
//     void set_color(Color _color);
//     void set_pos(Vector2 _pos);
//     // void set_size(Rectangle rect);
//     void set_min_progress(float _min_progress);
//     void set_max_progress(float _max_progress);
//     float get_progress();
//     void set_progress(float _progress);
//     void set_text(const std::string& _text);

//     void draw();
// };
