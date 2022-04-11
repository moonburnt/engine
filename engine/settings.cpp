#include "settings.hpp"
// For logging
#include "spdlog/spdlog.h"
// To save file to disk
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <fstream>

SettingsManager::SettingsManager(toml::table _default_settings, std::string path)
    : settings_path(path) {
    // Keep in mind that toml table does not keep order of items.
    // It may be annoying to manually edit tomls on disk due to that.
    default_settings = _default_settings;
    settings = default_settings;
}

void SettingsManager::save() {
    // ofstream is an output stream (ifstream would be input stream).
    // These don't need to be closed manually (unlike in C), coz close() is
    // called automatically on object's destruction (e.g when function ends)
    spdlog::info("Attempting to save settings to {}", settings_path);

    std::ofstream file;
    file.open(settings_path);
    file << settings;
}

void SettingsManager::reset() {
    settings = default_settings;
}

bool SettingsManager::load() {
    toml::table tbl;

    try {
        tbl = toml::parse_file(settings_path);
    } catch (const toml::parse_error& err) {
        // In case there is a parse error (say, file doesn't exist) - print it,
        // then attempt to write pre-existing settings (presumably default) to disks
        // spdlog::warn(fmt::format("{}", err));
        spdlog::warn(fmt::format("{}: {}", err.description(), err.source()));

        save();

        return 1;
    }

    bool overwrite_savefile = false;

    // This may go bananas with categories, will probably need some adjustments
    // TODO.
    for (auto&& [k, v] : default_settings) {
        // Adding values to savefile in case some are missing.
        // If addition has happened - schedule savefile overwriting
        auto status = tbl.insert(k, default_settings[k]);
        if (status.second) {
            overwrite_savefile = true;
        }
    }

    settings = tbl;

    if (overwrite_savefile) {
        save();
    }

    return 0;
}
