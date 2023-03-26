#include "menus.hpp"

#include "app.hpp"
#include "button_logic.hpp"
#include "spdlog/spdlog.h"

// #include <engine/ui.hpp>
#include "engine/ui/ui.hpp"
#include "engine/settings.hpp"

#include <functional>


// Playground Scene
PlaygroundScene::PlaygroundScene(App* app, LayerStorage* p, const std::string& _tag)
    : Scene({245, 245, 245, 255})
    , parent(p)
    , app(app)  {
    add_tag(_tag);
}


// Title Screen
TitleScreen::TitleScreen(App* app, LayerStorage* p)
    : PlaygroundScene(app, p, "Title Screen")
    , timer(Timer(2.0f)) {
    Node* ui_container = new Node();
    ui_container->set_align(Align::Center);

    // BasicTextNode* text = new BasicTextNode("This game has been made with raylib");
    UiText* text = new UiText("This game has been made with raylib");
    text->set_pos(
        {
            get_window_width() / 2.0f,
            get_window_height() / 2.0f
        }
    );
    ui_container->add_child(text);
    add_child(ui_container);

    timer.start();
}

void TitleScreen::update(float dt) {
    if (timer.tick(dt)) {
        // parent->set_current_scene(new MainMenu(app, parent));
        spdlog::info("Switching to main menu");
        parent->set_current(new MainMenu(app, parent));

    }
}

// Main Menu
MainMenu::MainMenu(App* app, LayerStorage* p)
    : PlaygroundScene(app, p, "Main Menu") {
    Node* ui_container = new Node();
    ui_container->set_align(Align::Center);

    // BasicTextNode* text = new BasicTextNode("Main Menu");
    UiText* text = new UiText("Main Menu");
    text->set_pos(
        {
            get_window_width() / 2.0f,
            100.0f,
        }
    );
    text->add_tag("MM");
    ui_container->add_child(text);

    TextButton* b = new TextButton("Start");
    b->set_pos(
        {
            get_window_width() / 2.0f,
            200.0f,
        }
    );
    b->add_tag("Start Button");
    ButtonStateSubject* clicked_subject = b->get_subject(ButtonState::Clicked);
    // clicked_subject->register_observer(new ShutdownObserver(app));
    SoundObserver* clicked_sound = new SoundObserver();
    clicked_sound->set_sound(app->assets.sounds["button_clicked"]);
    clicked_subject->register_observer(clicked_sound);

    ButtonStateSubject* hover_subject = b->get_subject(ButtonState::Hover);
    SoundObserver* hover_sound = new SoundObserver();
    hover_sound->set_sound(app->assets.sounds["button_hover"]);
    hover_subject->register_observer(hover_sound);

    TextObserver* text_idle_observer = new TextObserver(b);
    text_idle_observer->set_text("Idle");
    TextObserver* text_hover_observer = new TextObserver(b);
    text_hover_observer->set_text("Hover");
    TextObserver* text_pressed_observer = new TextObserver(b);
    text_pressed_observer->set_text("Pressed");
    TextObserver* text_clicked_observer = new TextObserver(b);
    text_clicked_observer->set_text("Clicked");
    b->get_subject(ButtonState::Hover)->register_observer(text_hover_observer);
    b->get_subject(ButtonState::Idle)->register_observer(text_idle_observer);
    b->get_subject(ButtonState::Pressed)->register_observer(text_pressed_observer);
    b->get_subject(ButtonState::Clicked)->register_observer(text_clicked_observer);

    ui_container->add_child(b);

    add_child(ui_container);
}

// // Settings Screen
// class SettingsScreen : public Scene {
// private:
//     SceneManager* parent;

//     // It may be done without this thing, but will do for now
//     toml::table current_settings;

//     Label title;
//     Label unsaved_changes_msg;
//     bool settings_changed;

//     Button* save_button;
//     Button* exit_button;

//     Label show_fps_title;
//     Checkbox* fps_cb;

//     Label fullscreen_title;
//     Checkbox* fullscreen_cb;

//     App* app;

//     void exit_to_menu() {
//         spdlog::info("Switching to main menu");
//         exit_button->reset_state();
//         parent->set_current_scene(new MainMenu(app, parent));
//     }

//     void save_settings() {
//         save_button->reset_state();
//         if (!settings_changed) return;

//         current_settings.insert_or_assign("show_fps", fps_cb->get_toggle());
//         current_settings.insert_or_assign("fullscreen", fullscreen_cb->get_toggle());

//         spdlog::info("Attempting to apply new settings");
//         settings_changed = false;

//         if (current_settings["show_fps"].value_exact<bool>().value()) {
//             if (app->window.sc_mgr.nodes.count("fps_counter") == 0) {
//                 app->window.sc_mgr.nodes["fps_counter"] = new FrameCounter();
//                 app->window.sc_mgr.nodes["fps_counter"]->set_pos({4.0f, 4.0f});
//             }
//         }
//         else {
//             app->window.sc_mgr.nodes.erase("fps_counter");
//         }

//         if (current_settings["fullscreen"].value_exact<bool>().value()) {
//             if (!IsWindowFullscreen()) {
//                 const int current_screen = GetCurrentMonitor();
//                 const int screen_width = GetMonitorWidth(current_screen);
//                 const int screen_height = GetMonitorHeight(current_screen);

//                 ToggleFullscreen();
//                 SetWindowSize(screen_width, screen_height);
//             };
//         }
//         else {
//             if (IsWindowFullscreen()) {
//                 SetWindowSize(
//                     current_settings["resolution"][0].value_or(1280),
//                     current_settings["resolution"][1].value_or(720));
//                 ToggleFullscreen();
//             };
//         }

//         app->config->settings = current_settings;
//         app->config->save();

//         spdlog::info("Resetting settings screen");
//         parent->set_current_scene(new SettingsScreen(app, parent));
//     }

// public:
//     SettingsScreen(App* app, SceneManager* p)
//         : parent(p)
//         , current_settings(app->config->settings) // this should get copied
//         , title("Settings", {get_window_width() / 2.0f, 30.0f})
//         , unsaved_changes_msg(
//               "Settings changed. Press save to apply!",
//               {get_window_width() / 2.0f, 60.0f})
//         , settings_changed(false)
//         , show_fps_title("Show FPS:", {30.0f, 100.0f})
//         , fullscreen_title("Fullscreen:", {30.0f, 150.0f})
//         , app(app) {

//         GuiBuilder b(app);
//         save_button = b.make_text_button("Save");
//         exit_button = b.make_close_button();

//         fps_cb = b.make_checkbox(
//             app->config->settings["show_fps"].value_exact<bool>().value());
//         fullscreen_cb = b.make_checkbox(
//             app->config->settings["fullscreen"].value_exact<bool>().value());

//         title.center();
//         unsaved_changes_msg.center();

//         save_button->set_pos(
//             {get_window_width() / 2.0f - save_button->get_rect().width / 2.0f,
//              get_window_height() - 100.0f});
//         exit_button->set_pos({static_cast<float>(get_window_width() - (30 + 64)), 30.0f});

//         const float cb_x = 200.0f;
//         fps_cb->set_pos({cb_x, 100.0f});
//         fullscreen_cb->set_pos({cb_x, 150.0f});
//     }

//     ~SettingsScreen() {
//         delete fps_cb;
//         delete fullscreen_cb;
//         delete exit_button;
//     }

//     void update(float) override {
//         save_button->update();
//         exit_button->update();
//         fps_cb->update();
//         fullscreen_cb->update();

//         if (exit_button->is_clicked()) {
//             exit_to_menu();
//             return;
//         }

//         if (save_button->is_clicked()) {
//             save_settings();
//             return;
//         }

//         if (fps_cb->is_clicked() || fullscreen_cb->is_clicked()) {
//             settings_changed = true;
//         }
//         else {
//             settings_changed = false;
//         }
//     }

//     void draw() override {
//         title.draw();

//         show_fps_title.draw();
//         fullscreen_title.draw();

//         save_button->draw();
//         exit_button->draw();
//         fps_cb->draw();
//         fullscreen_cb->draw();

//         if (settings_changed) {
//             unsaved_changes_msg.draw();
//         }
//     }
// };

// class TestRectangleNodeScene : public Scene {
// // private:
// //     SceneManager* parent;
// //     App* app;

// public:
//     TestRectangleNodeScene(App*, SceneManager*) {
//         // I think this should be destroyed in Node's destructor, so thats fine
//         add_child(new RectangleNode({50.0f, 50.0f, 50.0f, 50.0f}));
//     }

//     // void draw() override {}

// };

// // Main menu
// void MainMenu::call_exit() {
//     // parent->active = false;
//     app->window.quit();
// }

// void MainMenu::new_game() {
//     spdlog::info("Switching to level");
//     parent->set_current_scene(new TestRectangleNodeScene(app, parent));
// }

// void MainMenu::open_settings() {
//     spdlog::info("Switching to settings");
//     parent->set_current_scene(new SettingsScreen(app, parent));
// }

// MainMenu::MainMenu(App* app, SceneManager* p)
//     : parent(p)
//     , buttons(32.0f)
//     , app(app) {

//     buttons.set_pos({get_window_width() / 2.0f, get_window_height() / 2.0f});

//     GuiBuilder b(app);
//     buttons.add_button(b.make_text_button("New Game"));
//     buttons.add_button(b.make_text_button("Settings"));
//     buttons.add_button(b.make_text_button("Exit"));

//     buttons.center();
// }

// void MainMenu::update(float) {
//     // TODO: add keyboard controller, toggle manual update mode on and off,
//     // depending on what happend the last - some valid key press or mouse movement
//     buttons.update();

//     if (buttons[MM_NEWGAME]->is_clicked()) {
//         new_game();
//         return;
//     }

//     if (buttons[MM_SETTINGS]->is_clicked()) {
//         open_settings();
//         return;
//     }

//     if (buttons[MM_EXIT]->is_clicked()) {
//         call_exit();
//         return;
//     }
// }

// void MainMenu::draw() {
//     buttons.draw();
// }
