#include "storage.hpp"
#include "raylib.h"
#include <string>

void SpriteStorage::load(std::string path, std::string extension) {
    int amount;

    char** dir_files = GetDirectoryFiles(path.c_str(), &amount);

    for (auto current = 0; current < amount; current++) {
        if (IsFileExtension(dir_files[current], extension.c_str())) {
            std::string name_key(GetFileNameWithoutExt(dir_files[current]));
            items[name_key] = LoadTexture((path + dir_files[current]).c_str());
        }
    }

    ClearDirectoryFiles();
}

void SoundStorage::load(std::string path, std::string extension) {
    int amount;

    char** dir_files = GetDirectoryFiles(path.c_str(), &amount);

    for (auto current = 0; current < amount; current++) {
        if (IsFileExtension(dir_files[current], extension.c_str())) {
            std::string name_key(GetFileNameWithoutExt(dir_files[current]));
            items[name_key] = LoadSound((path + dir_files[current]).c_str());
        }
    }

    ClearDirectoryFiles();
}

// Idk if its good idea to keep all streams in memory. But will do for now. #TODO
void MusicStorage::load(std::string path, std::string extension) {
    int amount;

    char** dir_files = GetDirectoryFiles(path.c_str(), &amount);

    for (auto current = 0; current < amount; current++) {
        if (IsFileExtension(dir_files[current], extension.c_str())) {
            std::string name_key(GetFileNameWithoutExt(dir_files[current]));
            items[name_key] = LoadMusicStream((path + dir_files[current]).c_str());
        }
    }

    ClearDirectoryFiles();
}

MusicStorage::~MusicStorage() {
    for (auto& kv : items) {
        UnloadMusicStream(kv.second);
    }
}
