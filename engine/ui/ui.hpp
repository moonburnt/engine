#pragma once

#include "raylib.h"

#include <functional>
#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include "engine/utility.hpp"
#include "engine/text.hpp"
#include "engine/scene.hpp"
#include "components.hpp"

// Temporary, everything is implemented in header, for the sake of simplicity.
// TODO: move things out to .cpp once its done

// This will replace Label

class UiText: public RectangleNode {
protected:
    TextComponent text_component = TextComponent(this);

public:
    // TODO: construction with all parts customizable
    UiText(Text txt)
        : RectangleNode({0.0f, 0.0f, 0.0f, 0.0f}) {
            text_component.set_text(txt);
        }
    UiText(const std::string& txt)
        : RectangleNode({0.0f, 0.0f, 0.0f, 0.0f}) {
            text_component.set_text(txt);
        }

    const TextComponent& get_text_component() {
        return text_component;
    }

    std::string to_string() {
        return text_component.to_string();
    }

    void draw() override {
        text_component.draw();
    }
};


// FrameCounter from utility. Reworked.
class FrameCounter : public RectangleNode {
private:
    int fps_value = 0;

protected:
    TextComponent text_component = TextComponent(this);
    const char* format = "FPS: %02i";

public:
    // TODO: fully customizable settings
    // FrameCounter(const char* format, Font font, int size, int spacing, Color color);
    FrameCounter() : RectangleNode({0.0f, 0.0f, 0.0f, 0.0f}) {
        add_tag("FPS Counter");
    }

    void update(float) override {
        int new_fps = GetFPS();
        if (new_fps != fps_value) {
            fps_value = new_fps;
            text_component.set_text(TextFormat(format, fps_value));
        }
    }

    void draw() override {
        text_component.draw();
    }
};

