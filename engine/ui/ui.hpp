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
#include "engine/raybuff.hpp"
#include "components.hpp"
#include <type_traits>


// This will replace Label
class UiText: public RectangleNode {
protected:
    TextComponent text_component = TextComponent(this);

public:
    // TODO: construction with all parts customizable
    UiText(Text txt);
    UiText(const std::string& txt);

    const TextComponent& get_text_component();

    std::string get_string();

    void draw() override;
};

class Button : public RectangleNode {
protected:
    // TODO: move texture and text handling to the component's observer
    ButtonComponent button_comp = ButtonComponent(this);
    const Texture2D* texture = nullptr;

public:
    Button(Rectangle r);

    void set_texture(const Texture2D* t);
    void set_subject(ButtonState state, ButtonStateSubject* sub);

    ButtonStateSubject* get_subject(ButtonState state);

    void configure();

    // TODO: move state handling to component
    void update(float dt) override;

    std::string to_string() override;

    void draw() override;
};

// TODO: consider inheriting some things from UiText
class TextButton : public Button {
protected:
    TextComponent text_component = TextComponent(this);

public:
    TextButton(Text txt);
    TextButton(const std::string& txt);

    void set_text(const std::string& _txt);

    void draw() override;
};


// TODO: group these into a single debug overlay with configurable verbosity lvls
class FrameCounter : public RectangleNode {
private:
    int fps_value = 0;

protected:
    TextComponent text_component = TextComponent(this);
    const char* format = "FPS: %02i";

public:
    // TODO: fully customizable settings
    // FrameCounter(const char* format, Font font, int size, int spacing, Color color);
    FrameCounter();

    void update(float) override;
    void draw() override;
};

// MWP. TODO: split into components
class MousePosReporter: public RectangleNode {
private:
    Vector2 mouse_pos = {0.0f, 0.0f};

protected:
    TextComponent text_comp = TextComponent(this);
    const char* format = "Cursor: (%02i, %02i)";

public:
    MousePosReporter();

    void update(float) override;
    void draw() override;
};


class LayerStorage;

// This compiles, but I'm yet to see a successfull collision
class NodeInspector: public RectangleNode {
private:
    // Tree to report items from
    LayerStorage* report_from;

protected:
    TextComponent text_comp = TextComponent(this);

public:
    NodeInspector(LayerStorage* r);

    void update(float) override;
    void draw() override;
};
