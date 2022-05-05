#pragma once

#include <vector>
#include <algorithm>
#include <raylib.h>

// Idea is to group specific sounds together, allowing to adjust their volume,
// limit amount of concurrent sounds and so on.
//
// Current implementation is but barebones concept and is not used anywhere (since,
// for the time being, game has no music) - but I've tested it and it works.

template <typename T> class SimpleAudioManager {
protected:
    std::vector<T> currently_playing;
    bool is_playing;
    unsigned long int concurrent_sounds_limit;
    float volume;
    std::function<void(T, float)> set_audio_volume;
    std::function<void(T)> play_audio;
    std::function<void(T)> pause_audio;
    std::function<void(T)> resume_audio;
    std::function<bool(T)> is_audio_playing;

public:
    SimpleAudioManager(
        std::function<void(T, float)> _set_audio_volume,
        std::function<void(T)> _play_audio,
        std::function<void(T)> _pause_audio,
        std::function<void(T)> _resume_audio,
        std::function<bool(T)> _is_audio_playing,
        int slimit,
        float vol)
        : is_playing(true)
        , concurrent_sounds_limit(std::max(slimit, 1))
        , volume(std::clamp(vol, 0.0f, 1.0f)) {
        currently_playing.reserve(concurrent_sounds_limit);
        set_audio_volume = _set_audio_volume;
        play_audio = _play_audio;
        pause_audio = _pause_audio;
        resume_audio = _resume_audio;
        is_audio_playing = _is_audio_playing;
    }

    void play(T sound) {
        if (currently_playing.size() < concurrent_sounds_limit) {
            set_audio_volume(sound, volume);
            currently_playing.push_back(sound);
            play_audio(sound);
        }
    }

    void pause_all() {
        if (!is_playing) return;

        is_playing = false;
        for (auto sound : currently_playing) {
            pause_audio(sound);
        }
    }

    void resume_all() {
        if (is_playing) return;

        is_playing = true;
        for (auto sound : currently_playing) {
            resume_audio(sound);
        }
    }

    void update() {
        if (!is_playing) return;

        currently_playing.erase(
            std::remove_if(
                currently_playing.begin(),
                currently_playing.end(),
                [this](T sound) { return !is_audio_playing(sound); }),
            currently_playing.end());
    }
};

class SoundManager : public SimpleAudioManager<Sound> {
public:
    SoundManager(int slimit, float vol)
        : SimpleAudioManager(
            SetSoundVolume,
            PlaySound,
            PauseSound,
            ResumeSound,
            IsSoundPlaying,
            slimit,
            vol) {}

    SoundManager()
        : SoundManager(5, 1.0f) {}
};

// TODO: music manager may also have additional effects, such as tracks crossfading.

class MusicManager : public SimpleAudioManager<Music> {
public:
    MusicManager(int slimit, float vol)
        : SimpleAudioManager(
            SetMusicVolume,
            PlayMusicStream,
            PauseMusicStream,
            ResumeMusicStream,
            IsMusicStreamPlaying,
            slimit,
            vol) {}

    MusicManager()
        : MusicManager(2, 1.0f) {}

    void update() {
        for (auto sound: currently_playing) {
            UpdateMusicStream(sound);
        }
    }
};
