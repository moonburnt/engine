#include "app.hpp"
#include "platform.hpp"
#include "menus.hpp"

#include "engine/utility.hpp"
#include "engine/ui/ui.hpp"

#include "fmt/format.h"

App::App() {

    platform = Platform::make_platform();

    auto resource_dir = platform->get_resource_dir();
    auto settings_dir = platform->get_settings_dir();

    config = std::make_unique<SettingsManager>(
        toml::table{
            {"show_fps", true},
            {"fullscreen", false},
            {"resolution", toml::array{1280, 720}},
            {"sfx_volume", 100},
            {"music_volume", 100}},
        fmt::format("{}settings.toml", settings_dir));

    config->load();

    window.init(
        std::max(config->settings["resolution"][0].value_or(1280), 1280),
        std::max(config->settings["resolution"][1].value_or(720), 720),
        "Engine Playground");

    if (config->settings["fullscreen"].value_or(false) && !IsWindowFullscreen()) {
        // TODO: add ability to specify active monitor
        const int current_screen = GetCurrentMonitor();
        // Its important to first toggle fullscreen and only them apply size.
        // Else this won't work (I spent 3 hours trying to debug this)
        ToggleFullscreen();
        SetWindowSize(GetMonitorWidth(current_screen), GetMonitorHeight(current_screen));
    };

    assets.sprites.load(platform->get_sprites_dir(), ".png");
    assets.sounds.load(platform->get_sounds_dir(), ".ogg");
}

void App::run() {
    LayerStorage* scenes = window.sc_mgr.configure_layer("scenes");
    LayerStorage* overlay = window.sc_mgr.configure_layer("overlay");

    if (config->settings["show_fps"].value_or(false)) {
        overlay->get_current_or_future()->add_child(new FrameCounter());
    };

    // TODO: add ability to show/hide and stop/play nodes;
    // Then make this toggle on/off by, say , F9
    overlay->get_current_or_future()->add_child(new MousePosReporter());
    overlay->get_current_or_future()->add_child(new NodeInspector(scenes));

    scenes->set_current(new TitleScreen(this, scenes));

    window.run();
}
