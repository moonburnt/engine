#pragma once

#include <vector>

#include <raylib.h>

// Idea is to group specific sounds together, allowing to adjust their volume,
// limit amount of concurrent sounds and so on.
// Something like manager for Music type may also have additional effects, such
// as tracks crossfading.
//
// Current implementation is but barebones concept and is not used anywhere (since,
// for the time being, game has no music) - but I've tested it and it works.
class SoundManager {
protected:
    std::vector<Sound> currently_playing;
    bool is_playing;
    unsigned long int concurrent_sounds_limit;
    float volume;

public:
    SoundManager();
    SoundManager(int slimit, float vol);

    void play(Sound sound);

    void pause_all();
    void resume_all();

    void update();

    static SoundManager mgr;
};
