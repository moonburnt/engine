#pragma once

#include "engine/core.hpp"
#include "engine/ui/components.hpp"
#include "engine/ui/ui.hpp"

// Common button state change handlers

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
