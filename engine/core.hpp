#pragma once

#include "raylib.h"
#include "sound.hpp"
#include "scene.hpp"

// TODO: think about moving MusicManager out of GameWindow, moving App from
// playground to mainline engine and leaving GameWindow with only
// window-management functionality, while App holds everything together.
class GameWindow {
protected:
    bool initialized = false;
    bool active = false;

public:
    SceneManager sc_mgr;
    MusicManager music_mgr;

    GameWindow(SceneManager sc_mgr, MusicManager music_mgr);
    GameWindow();

    // Initialize game window
    void init(int x, int y, const std::string &title, int fps);
    void init(int x, int y, const std::string &title);
    void init(int x, int y);
    void init();

    bool is_active();
    // Run scene manager and other stuff
    void run();
    void quit();
};
