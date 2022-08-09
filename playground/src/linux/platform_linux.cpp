#include "platform_linux.hpp"

std::string PlatformLinux::get_resource_dir() {
    return "./Assets/";
}

std::string PlatformLinux::get_sprites_dir() {
    return get_resource_dir() + "Sprites/";
}

std::string PlatformLinux::get_sounds_dir() {
    return get_resource_dir() + "SFX/";
}

std::string PlatformLinux::get_settings_dir() {
    return "./";
}
