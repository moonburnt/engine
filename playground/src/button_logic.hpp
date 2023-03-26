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

// class SoundObserver: public ButtonStateObserver {
// public:
//     ShutdownHandler(App* _app): app(_app) {}

//     void update(float dt) override {
//         spdlog::info("Playing the sound");
//         app->window.quit();
//     }
// };
