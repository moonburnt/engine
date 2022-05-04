#pragma once

#include <raylib.h>
#include <string>
#include <unordered_map>
#include <vector>
// To find() things in InputHandler
#include "core.hpp"
#include <algorithm>

static constexpr uint32_t DEFAULT_TEXT_SIZE = 20u;
static constexpr Color DEFAULT_TEXT_COLOR = BLACK;

void handle_assert(const char* file, int line, const char* fun, const char* expr);

#define ASSERT(expr)                                                                     \
    while (!(expr)) {                                                                    \
        handle_assert(__FILE__, __LINE__, __FUNCTION__, #expr);                          \
        break;                                                                           \
    }

// Basic timer to use with tasks that should be executed based on frame time passed.
// Must be updated each frame manually with tick() (which I may rename later to
// update(), for sake of consistence with other objects)
class Timer {
private:
    bool started;
    bool completed;
    float duration;
    float time_left;

public:
    Timer(float length);
    // Start/restart timer.
    void start();
    // Update timer for specific value. Returns completion status.
    bool tick(float dt);
    // Stop timer.
    void stop();
    // Get timer's startup status.
    bool is_started();
};

// Abstract input controller that holds relationship between keys and enum of
// actions related to them. There may be multiple of these across the program,
// depending on context that require one or another controls scheme (say, one to
// handle player movement, other to handle ui, etc etc)
template <typename ActionEnum> class InputController {
public:
    // Bind specific key to specific action and start tracking it.
    // Key must be part of raylib's keys enum, action - part of your own enum of
    // actions, which you wrote down specifically for particular controller type.
    void add_relationship(int key, ActionEnum action) {
        key_binds[key] = action;
    }

    // Update list of held buttons
    void update() {
        for (auto& kv : key_binds) {
            if (IsKeyDown(kv.first)) {
                if (!std::count(buttons_held.begin(), buttons_held.end(), kv.first)) {
                    buttons_held.push_back(kv.first);
                }
            }
            else {
                auto it = std::find(buttons_held.begin(), buttons_held.end(), kv.first);
                if (it != buttons_held.end()) buttons_held.erase(it);
            }
        }
    }

    // Reset buttons_held, to forget whats been pressed. Doesn't clean up key_binds.
    void reset_active() {
        buttons_held.clear();
    }

    // Get action of button just held. Returns 0 if no valid button presses has
    // been registered, thus your enum should start with none/nothing.
    // May rework that to -1 in future.
    ActionEnum get_action() {
        if (buttons_held.empty()) return static_cast<ActionEnum>(0);
        else return key_binds[buttons_held.back()];
    }

private:
    // List of buttons held simultaneously right now. May be empty, if no keys
    // from key_binds are pressed right now. Last button pressed should always
    // appear at the end of vector.
    std::vector<int> buttons_held;

    // Tracked buttons. First int is key, second - action.
    std::unordered_map<int, ActionEnum> key_binds;
};

// Get position that will be perfect text's center.
Vector2 center_text(const std::string& text, Vector2 center);

// Get position that will be perfect text's horizontal center.
int center_text_h(const std::string& text, int center);

// Center rectangle in place, overriding its existing position.
void center_rect_ip(Rectangle* rect, Vector2 center);

// Get random bool value
bool randbool();

// Returns true if mouse position has been changed this frame, false otherwise.
// It may be wiser to move it somewhere else. TODO
bool has_mouse_moved();

// Returns true if second rect is inside first rect, false if some part of it
// is outside of borders.
bool is_rect_inside_rect(Rectangle first, Rectangle second);

// TODO: customizable font
class FrameCounter : public Node {
private:
    int fps_value;

protected:
    Vector2 pos;
    const char* format;
    int size;
    Color color;

public:
    FrameCounter(Vector2 pos, const char* format, int size, Color color);
    FrameCounter(Vector2 pos, int size);
    FrameCounter(Vector2 pos);
    FrameCounter();

    void update() override;
    void draw() override;
};
