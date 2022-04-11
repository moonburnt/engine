#include "sound.hpp"
#include <algorithm>
#include <raylib.h>

SoundManager SoundManager::mgr;

SoundManager::SoundManager()
    : is_playing(true)
    , concurrent_sounds_limit(5)
    , volume(1.0f) {
    currently_playing.reserve(concurrent_sounds_limit);
}

SoundManager::SoundManager(int slimit, float vol)
    : is_playing(true)
    , concurrent_sounds_limit(std::max(slimit, 1))
    , volume(std::clamp(vol, 0.0f, 1.0f)) {
    currently_playing.reserve(concurrent_sounds_limit);
}

void SoundManager::play(Sound sound) {
    if (currently_playing.size() < concurrent_sounds_limit) {
        SetSoundVolume(sound, volume);
        currently_playing.push_back(sound);
        PlaySound(sound);
    }
}

void SoundManager::pause_all() {
    if (!is_playing) return;

    is_playing = false;
    for (auto sound : currently_playing) {
        PauseSound(sound);
    }
}

void SoundManager::resume_all() {
    if (is_playing) return;

    is_playing = true;
    for (auto sound : currently_playing) {
        ResumeSound(sound);
    }
}

void SoundManager::update() {
    if (!is_playing) return;

    currently_playing.erase(
        std::remove_if(
            currently_playing.begin(),
            currently_playing.end(),
            [](Sound sound) { return !IsSoundPlaying(sound); }),
        currently_playing.end());
}
