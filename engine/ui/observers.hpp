#pragma once

#include "raylib.h"

#include <algorithm>
#include <functional>
#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include "engine/utility.hpp"
#include "engine/text.hpp"
#include "engine/scene.hpp"
#include "engine/observer.hpp"
#include "engine/ui/ui.hpp"

// Common button state change observers

class ShutdownObserver: public ButtonStateObserver {
private:
    App* app = nullptr;

public:
    ShutdownObserver(App* a): app(a) {}

    void update(float dt) override {
        spdlog::info("Calling game's shutdown");
        app->window.quit();
    }
};

class SoundObserver: public ButtonStateObserver {
private:
    const Sound* sound = nullptr;
public:
    void set_sound(const Sound* s) {
        sound = s;
    }

    void update(float dt) override {
        if (sound != nullptr) {
            PlaySound(*sound);
        }
    }
};

class TextObserver: public ButtonStateObserver {
private:
    TextButton* button = nullptr;
    std::string txt = "";

public:
    TextObserver(TextButton* b) : button(b) {}

    void set_text(const std::string& _txt) {
        txt = _txt;
    }

    void update(float dt) override {
        if (button) {
            button->set_text(txt);
        }
    }
};


class TextureObserver: public ButtonStateObserver {
private:
    const Texture* texture = nullptr;
    Button* button = nullptr;

public:
    void attach_to_button(Button* b) {
        button = b;
    }

    void set_texture(const Texture* t) {
        texture = t;
    }

    void update(float dt) override {
        if ((texture != nullptr) && (button != nullptr)) {
            button->set_texture(texture);
        }
    }
};
