#pragma once

#include "toml++/toml.h"
#include <string>

class SettingsManager {
private:
    std::string settings_path;

    toml::table default_settings;

public:
    SettingsManager(toml::table default_settings, const std::string &settings_path);

    toml::table settings;

    // Save settings to file on provided path
    void save();
    // Load settings from provided path
    bool load();
    // Reset settings to defaults
    void reset();
};
