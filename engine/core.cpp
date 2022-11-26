#include "core.hpp"
// For logging
#include "spdlog/spdlog.h"
// For vsprintf
#include <cstdio>

void TraceLog(int logLevel, const char* text, va_list args) {
    static char log_text[2048] = {0}; // I think 2048 would be enough?
    // Doing this via std, coz fmt's function went bananas on some types.
    std::vsprintf(log_text, text, args);

    switch (logLevel) {
    case LOG_TRACE:
        spdlog::trace(log_text);
        break;
    case LOG_DEBUG:
        spdlog::debug(log_text);
        break;
    case LOG_INFO:
        spdlog::info(log_text);
        break;
    case LOG_WARNING:
        spdlog::warn(log_text);
        break;
    case LOG_ERROR:
        spdlog::error(log_text);
        break;
    case LOG_FATAL:
        spdlog::critical(log_text);
        break;
    default:
        spdlog::info(log_text);
        break;
    }
}

// GameWindow stuff
GameWindow::GameWindow(SceneManager sc_mgr, MusicManager music_mgr)
    : sc_mgr(sc_mgr)
    , music_mgr(music_mgr) {
    // Overriding raylib's default logger to pipe messages to spdlog.
    // Doing this to provide unified look and management for both engine and
    // custom messages.
    SetTraceLogCallback(TraceLog);
    spdlog::set_pattern("[%H:%M:%S][%l] %v");
}

GameWindow::GameWindow()
    : GameWindow(SceneManager(), MusicManager()) {
}

void GameWindow::init(int x, int y, const std::string &title, int fps) {
    if (initialized) {
        spdlog::warn("GameWindow has already been initialized.");
        return;
    }
    spdlog::info("Initializing GameWindow.");

    InitWindow(x, y, title.c_str());
    // Setting window's framerate
    SetTargetFPS(fps);
    // Disable ability to close the window by pressing esc
    SetExitKey(KEY_NULL);

    // Initialize audio device. This needs to be done before loading sounds.
    InitAudioDevice();

    initialized = true;
}

void GameWindow::init(int x, int y, const std::string &title) {
    init(x, y, title, 60);
}

void GameWindow::init(int x, int y) {
    init(x, y, "Game Window");
}

void GameWindow::init() {
    init(1280, 720);
}

bool GameWindow::is_active() {
    return active && !WindowShouldClose();
}

void GameWindow::run() {
    if (!initialized) {
        spdlog::critical("Attempting to run unitialized GameWindow.\n"
                         "Did you forget GameWindow.init()?");
        abort();
    };

    if (active) {
        spdlog::warn("GameWindow is already running.");
        return;
    }

    active = true;

    while (is_active()) {
        // Because we don't really need double precision there
        sc_mgr.update(static_cast<float>(GetFrameTime()));
        music_mgr.update();
    }

    spdlog::info("Attempting to shutdown the game.");
    CloseWindow();
}

void GameWindow::quit() {
    active = false;
}
