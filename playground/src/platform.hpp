#pragma once

#include <memory>
#include <string>

class Platform {
public:
    static std::unique_ptr<Platform> make_platform();

    virtual std::string get_resource_dir() = 0;
    virtual std::string get_sprites_dir() = 0;
    virtual std::string get_sounds_dir() = 0;
    virtual std::string get_settings_dir() = 0;

    virtual ~Platform() = default;
};
