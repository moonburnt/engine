#include "storage.hpp"
#include "raylib.h"
#include <string>

void SpriteStorage::load(const std::string &path, const std::string &extension) {
    load_with(path, extension, LoadTexture);
}

SpriteStorage::~SpriteStorage() {
    for (const auto &kv: items) {
        UnloadTexture(kv.second);
    }
}

void SoundStorage::load(const std::string &path, const std::string &extension) {
    load_with(path, extension, LoadSound);
}

SoundStorage::~SoundStorage() {
    for (const auto &kv: items) {
        UnloadSound(kv.second);
    }
}

// Idk if its good idea to keep all streams in memory. But will do for now. #TODO
void MusicStorage::load(const std::string &path, const std::string &extension) {
    load_with(path, extension, LoadMusicStream);
}

MusicStorage::~MusicStorage() {
    for (auto& kv : items) {
        UnloadMusicStream(kv.second);
    }
}
