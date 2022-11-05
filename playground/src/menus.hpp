#pragma once

#include "engine/core.hpp"
#include "engine/ui.hpp"
#include "engine/utility.hpp"

class App;

class TitleScreen : public Scene {
private:
    SceneManager* parent;
    Timer timer;
    // Label greeter;
    App* app;

public:
    TitleScreen(App* app, SceneManager* p);

    void update(float dt) override;
    void draw() override;
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
