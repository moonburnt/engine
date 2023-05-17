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

class GameWindow;

class ShutdownObserver: public ButtonStateObserver {
private:
    GameWindow* window;

public:
    ShutdownObserver(GameWindow* w);

    void update(float) override;
};

class SoundObserver: public ButtonStateObserver {
private:
    const Sound* sound = nullptr;
public:
    void set_sound(const Sound* s);

    void update(float) override;
};

class TextObserver: public ButtonStateObserver {
private:
    TextButton* button = nullptr;
    std::string txt = "";

public:
    TextObserver(TextButton* b);

    void set_text(const std::string& _txt);
    void update(float) override;
};


class TextureObserver: public ButtonStateObserver {
private:
    const Texture* texture = nullptr;
    Button* button = nullptr;

public:
    void attach_to_button(Button* b);
    void set_texture(const Texture* t);

    void update(float) override;
};
