#pragma once

#include "engine/core.hpp"
#include "engine/ui.hpp"
#include "engine/utility.hpp"

class App;

class PlaygroundScene : public Scene {
protected:
    LayerStorage* parent;
    App* app;
public:
    PlaygroundScene(App* app, LayerStorage* p, const std::string& _tag);
};

class TitleScreen : public PlaygroundScene {
private:
    Timer timer;

public:
    TitleScreen(App* app, LayerStorage* p);

    void update(float dt) override;
};

class MainMenu : public PlaygroundScene {
private:
    enum class MM_BUTTONS{
        MM_NEWGAME,
        MM_SETTINGS,
        MM_EXIT,
        MM_CONTINUE
    };

    std::unordered_map<MM_BUTTONS, Node*> buttons;
public:
    MainMenu(App* app, LayerStorage* p);
};

// class MainMenu : public Scene {
// private:
//     enum MM_BUTTONS {
//         MM_NEWGAME,
//         MM_SETTINGS,
//         MM_EXIT,
//         MM_CONTINUE
//     };

//     SceneManager* parent;
//     VerticalContainer buttons;
//     App* app;

//     void call_exit();
//     void new_game();
//     void open_settings();

// public:
//     MainMenu(App* app, SceneManager* p);

//     void update(float) override;
//     void draw() override;
// };
