#pragma once

#include "toml++/toml.h"
#include <string>

// TODO:
// - Add default engine's settings, initialized with SettingsManager.
// Such settings will contain things iike debug output, hitboxes showcase, etc.
// - Store a SettingsManager instance in GameWindow
// - Make engine use these settings where possible.
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
