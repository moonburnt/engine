#include "observers.hpp"


ShutdownObserver::ShutdownObserver(GameWindow* w): window(w) {}

void ShutdownObserver::update(float) {
    spdlog::info("Calling game's shutdown");
    window->quit();
}


void SoundObserver::set_sound(const Sound* s) {
    sound = s;
}

void SoundObserver::update(float) {
    if (sound != nullptr) {
        PlaySound(*sound);
    }
}


TextObserver::TextObserver(TextButton* b) : button(b) {}

void TextObserver::set_text(const std::string& _txt) {
    txt = _txt;
}

void TextObserver::update(float) {
    if (button) {
        button->set_text(txt);
    }
}


void TextureObserver::attach_to_button(Button* b) {
    button = b;
}

void TextureObserver::set_texture(const Texture* t) {
    texture = t;
}

void TextureObserver::update(float) {
    if ((texture != nullptr) && (button != nullptr)) {
        button->set_texture(texture);
    }
}
