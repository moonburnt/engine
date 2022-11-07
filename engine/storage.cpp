#include "storage.hpp"
#include "raylib.h"
#include <string>


Texture2D SpriteStorage::load_data(const std::string &path) {
    return LoadTexture(path.c_str());
}

void SpriteStorage::unload_data(Texture2D data) {
    UnloadTexture(data);
}


Sound SoundStorage::load_data(const std::string &path) {
    return LoadSound(path.c_str());
}

void SoundStorage::unload_data(Sound data) {
    UnloadSound(data);
}


// Idk if its good idea to keep all streams in memory. But will do for now. TODO
Music MusicStorage::load_data(const std::string &path) {
    return LoadMusicStream(path.c_str());
}

void MusicStorage::unload_data(Music data) {
    UnloadMusicStream(data);
}
