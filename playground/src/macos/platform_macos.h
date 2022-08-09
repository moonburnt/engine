#pragma once

#include "platform.hpp"

class PlatformMacos : public Platform {
public:
    std::string get_resource_dir() override;
    std::string get_sprites_dir() override;
    std::string get_sounds_dir() override;
    std::string get_settings_dir() override;
};
