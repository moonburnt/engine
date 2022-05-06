#pragma once

#include <algorithm>
#include <deque>
#include <functional>
#include "raylib.h"

// Idea is to group specific sounds together, allowing to adjust their volume,
// limit amount of concurrent sounds and so on.
//
// Current implementation is but barebones concept and is not used anywhere (since,
// for the time being, game has no music) - but I've tested it and it works.

template <typename T> class SimpleAudioManager {
protected:
    std::deque<T> currently_playing;
    bool is_playing;
    // This should probably be less than the following:
    // https://github.com/raysan5/raylib/blob/ed29b4eedf0a5cea5a7878658d50743088dba6a6/src/raudio.c#L268
    unsigned long int concurrent_sounds_limit;
    float volume;
    std::function<void(T, float)> set_audio_volume;
    std::function<void(T)> play_audio;
    std::function<void(T)> pause_audio;
    std::function<void(T)> stop_audio;
    std::function<void(T)> resume_audio;
    std::function<bool(T)> is_audio_playing;

public:
    SimpleAudioManager(
        std::function<void(T, float)> _set_audio_volume,
        std::function<void(T)> _play_audio,
        std::function<void(T)> _pause_audio,
        std::function<void(T)> _stop_audio,
        std::function<void(T)> _resume_audio,
        std::function<bool(T)> _is_audio_playing,
        int slimit,
        float vol)
        : is_playing(true)
        , concurrent_sounds_limit(std::max(slimit, 1))
        , volume(std::clamp(vol, 0.0f, 1.0f)) {
        // There is no reserve() for deque, since it would bring no benefits
        // currently_playing.reserve(concurrent_sounds_limit);
        set_audio_volume = _set_audio_volume;
        play_audio = _play_audio;
        pause_audio = _pause_audio;
        stop_audio = _stop_audio;
        resume_audio = _resume_audio;
        is_audio_playing = _is_audio_playing;
    }

    void set_volume(float vol) {
        volume = std::clamp(vol, 0.0f, 1.0f);
    }

    // Set concurrent sounds limit. If less than current amount of tracks -
    // will forcibly stop and remove all older tracks from queue
    void set_concurrent_sounds_limit(size_t limit) {
        if (limit < 1) {
            return;
        }

        if (currently_playing.size() > limit) {
            int limit_diff = limit - currently_playing.size();

            // I think this should work?
            for (auto i = 0; i < limit_diff; i++) {
                stop_audio(currently_playing.back());
                currently_playing.pop_back();
            }
        }

        concurrent_sounds_limit = limit;
    }

    void play(T sound, bool replace_old) {
        if (currently_playing.size() >= concurrent_sounds_limit) {
            if (replace_old) {
                // Hopefully this wont cause trouble in threads...
                stop_audio(currently_playing.back());
                // Hopefully this wont be casted on an empty container...
                currently_playing.pop_back();
            }
            else {
                return;
            }
        }

        is_playing = true;
        set_audio_volume(sound, volume);
        currently_playing.push_front(sound);
        play_audio(sound);
    }

    void play(T sound) {
        play(sound, false);
    }

    void play_all() {
        if (is_playing) return;

        is_playing = true;
        for (auto sound : currently_playing) {
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

    void stop_all() {
        if (!is_playing) return;

        is_playing = false;
        for (auto sound : currently_playing) {
            stop_audio(sound);
        }
    }

    void reset() {
        stop_all();
        currently_playing.clear();
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
              StopSound,
              ResumeSound,
              IsSoundPlaying,
              slimit,
              vol) {
    }

    SoundManager()
        : SoundManager(5, 1.0f) {
    }
};

// TODO: music manager may also have additional effects, such as tracks crossfading.

class MusicManager : public SimpleAudioManager<Music> {
public:
    MusicManager(int slimit, float vol)
        : SimpleAudioManager(
              SetMusicVolume,
              PlayMusicStream,
              PauseMusicStream,
              StopMusicStream,
              ResumeMusicStream,
              IsMusicStreamPlaying,
              slimit,
              vol) {
    }

    MusicManager()
        : MusicManager(2, 1.0f) {
    }

    void update() {
        for (auto sound : currently_playing) {
            UpdateMusicStream(sound);
        }
        SimpleAudioManager<Music>::update();
    }
};
