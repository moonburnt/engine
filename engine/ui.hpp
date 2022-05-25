#pragma once

#include <raylib.h>

#include <functional>
#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include "utility.hpp"

// UI primitives.
enum class ButtonStates {
    idle,
    hover,
    pressed,
    clicked,
    disabled
};

class ButtonBase {
public:
    virtual ButtonStates update() = 0;
    virtual void draw() = 0;
    virtual void set_manual_update_mode(bool mode) = 0;
    virtual void reset_state() = 0;
    virtual void set_state(ButtonStates state) = 0;
    virtual void set_pos(Vector2 position) = 0;
    virtual ButtonStates get_state() = 0;
    virtual Rectangle get_rect() = 0;
    virtual bool is_clicked() = 0;
    virtual Vector2 get_pos() = 0;

    virtual ~ButtonBase() = default;
};

// Basic label.
class Label {
protected:
    std::string text;
    int text_size = DEFAULT_TEXT_SIZE;
    Color text_color = DEFAULT_TEXT_COLOR;

    // Pos set by set_pos and on init
    Vector2 pos;
    // Pos to draw on, that may be different if center() has been used.
    Vector2 real_pos;

public:
    Label(const std::string& txt, Vector2 position);

    // Center message around its position
    void center();

    void set_pos(Vector2 pos, bool center);
    void set_pos(Vector2 pos);
    Vector2 get_pos();

    // Set label's text. May need to re-center message after that.
    void set_text(const std::string& txt);

    void draw();
};

// Simple text input field, for basic text editing.
class TextInputField : public Label {
protected:
    // Text shown on background if no input has been received
    std::string bg_text;
    Vector2 bg_text_pos;
    int bg_text_size = DEFAULT_TEXT_SIZE;
    Color bg_text_color = LIGHTGRAY;
    // Max amount of characters in text box. 0 means "unlimited".
    // This only affects text received from input, not bg_text.
    size_t max_size = 0;

    std::string blink_char;
    Vector2 blink_pos;
    bool draw_blink = false;
    Timer blink_timer;

    void update_blink_pos();

public:
    // default_text is text that will be set as bg_text
    TextInputField(
        const std::string& default_text,
        Vector2 pos,
        size_t max_size,
        const char blink_char,
        float blink_frequency);
    TextInputField(const std::string& default_text, Vector2 pos, size_t max_size);
    TextInputField(const std::string& default_text, Vector2 pos);

    void set_text(const std::string& txt);
    void set_pos(Vector2 pos, bool center);
    void set_pos(Vector2 pos);

    // Default input field update method.
    // TODO: add support for gamepad's virtual keyboard
    void update(float dt);
    void draw();
};

class Button : public ButtonBase {
protected:
    // Button position. Hitbox will always appear at center of it
    Vector2 pos;
    // State is current button state
    ButtonStates state;
    // State update mode. See set_manual_update_mode();
    bool manual_update_mode;
    std::unordered_map<ButtonStates, const Texture2D*> textures;

    // Callback function to be triggered if button has been clicked
    std::function<void()> on_click_callback = nullptr;

    Label* text = nullptr;

private:
    std::unordered_map<ButtonStates, const Sound*> sounds;
    // Button's hitbox. X and Y are offsets from texture's top left
    Rectangle rect;
    // Button state from previous screen
    ButtonStates last_state;

public:
    Button(
        std::unordered_map<ButtonStates, const Texture2D*> textures,
        std::unordered_map<ButtonStates, const Sound*> sounds,
        Rectangle rectangle);

    Button(
        const std::string& text,
        std::unordered_map<ButtonStates, const Texture2D*> textures,
        std::unordered_map<ButtonStates, const Sound*> sounds,
        Rectangle rectangle);

    ~Button();

    void enable();
    void disable();
    bool is_enabled();

    // Set on_click_callback function
    void set_callback(std::function<void()> on_click_callback);

    void set_text(const std::string& txt);

    ButtonStates update() override;
    void draw() override;

    // Set manual update mode. If true - won't update state with update(),
    // instead will only respond to manual set_state() calls.
    void set_manual_update_mode(bool mode) override;
    // Override button state. Will also toggle on manual update mode.
    void set_state(ButtonStates state) override;
    ButtonStates get_state() override;
    // Reset button state and last_state to ButtonStates::idle.
    void reset_state() override;
    void set_pos(Vector2 position) override;
    bool is_clicked() override;
    // This getter is necessary to use pos from outside without being able
    // to overwrite its value directly (coz its meant to be done together
    // with changing position of other button's elements)
    Vector2 get_pos() override;
    // Same there
    Rectangle get_rect() override;
};

class Checkbox : public Button {
private:
    // Storage for textures of disabled checkbox
    std::unordered_map<ButtonStates, const Texture2D*> textures_off;
    // State of checkbox (on or off)
    bool toggle_state;
    // Has checkbox'es state been changed with toggle() or not.
    bool state_switched;

public:
    Checkbox(
        std::unordered_map<ButtonStates, const Texture2D*> textures_on,
        std::unordered_map<ButtonStates, const Texture2D*> textures_off,
        std::unordered_map<ButtonStates, const Sound*> sounds,
        Rectangle rectangle);

    Checkbox(
        std::unordered_map<ButtonStates, const Texture2D*> textures_on,
        std::unordered_map<ButtonStates, const Texture2D*> textures_off,
        std::unordered_map<ButtonStates, const Sound*> sounds,
        Rectangle rectangle,
        bool default_state);

    // Returns toggle state of checkbox
    bool get_toggle();
    // Toggle checkbox on or off. Doesn't affect "state_switched".
    void toggle(bool toggle_state);
    // Toggle checkbox state back and forth. Toggles "state_switched", until
    // reset_state() is used.
    void toggle();

    void draw() override;
    ButtonStates update() override;

    // Returns state_switched.
    bool is_clicked() override;
    // Resets state_switched.
    void reset_state() override;
};

class ButtonStorage {
protected:
    // Storage that holds buttons. Since there is no way to remove these, its
    // a vector. May rework into map if there will be need to remove elements.
    std::vector<ButtonBase*> storage;
    // Number of selected button in storage
    // It may be an overkill to use optional there, but otherwise there would be
    // issues with setting unsettable default that will compile.
    std::optional<size_t> selected_button;
    bool manual_update_mode;

public:
    ButtonStorage();
    ~ButtonStorage();
    // Add button or its derivatives to storage.
    virtual void add_button(ButtonBase* button);
    void set_manual_update_mode(bool mode);
    // Select button with specific id in storage. Any of these selectors will
    // automatically toggle on set_manual_update_mode().
    void select_button(size_t button);
    void select_next(bool cycle);
    void select_previous(bool cycle);
    // Update all buttons assigned to storage. If manual_update_mode has been set
    // to true - only updates selected button.
    void update();
    // Set state of selected button. Returns true if there is such button, else false
    bool set_selected_button_state(ButtonStates state);
    // Draw all buttons in storage
    void draw();

    std::optional<std::tuple<int, ButtonStates>> get_selected_button_state();

    // Overload to [], that provides access to storage elements.
    // For now, doesn't have safety checks - requiesting out of bounds value will
    // probably lead to segfault.
    ButtonBase* operator[](int i);
    ButtonBase* at(int i);
};

class VerticalContainer : public ButtonStorage {
private:
    // Container position. Rect will always appear at center of it
    Vector2 pos = {0.0f, 0.0f};

    // Container's own rectangle. Its size change dynamically on addition of
    // new items. Width is equal to width of widest element and height - height
    // of all elements total.
    Rectangle rect = {0.0f, 0.0f, 0.0f, 0.0f};

    // Gap between items
    float gap;

public:
    VerticalContainer(float gap);

    void set_pos(Vector2 pos);

    void center();

    void add_button(ButtonBase* button) override;
};
