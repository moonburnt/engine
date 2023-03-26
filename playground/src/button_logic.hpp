#pragma once

#include "engine/core.hpp"
#include "engine/ui/components.hpp"

// Common button state change handlers

class ShutdownObserver: public ButtonStateObserver {
private:
    App* app = nullptr;

public:
    ShutdownObserver(App* _app): app(_app) {}

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
