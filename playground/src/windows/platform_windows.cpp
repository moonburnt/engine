#include "platform_windows.hpp"

std::string PlatformWindows::get_resource_dir() {
    return "../Assets/";
}

std::string PlatformWindows::get_sprites_dir() {
    return get_resource_dir() + "Sprites/";
}

std::string PlatformWindows::get_sounds_dir() {
    return get_resource_dir() + "SFX/";
}

std::string PlatformWindows::get_settings_dir() {
    return "./";
}
