#pragma once

#include "raylib.h"

#include <string>
#include <unordered_map>
#include <functional>


template <typename T> class Storage {
protected:
    std::unordered_map<std::string, T> items;

    // Data loader. Pure-virtual, implementation-dependant.
    // Designed to wrap calls to raylib functions, but may work differently.
    virtual T load_data(const std::string &path) = 0;

    // Data unloader. Same as above, but not pure-virtual to avoid UB
    // Not quite sure if it work rn tho. TODO
    virtual void unload_data(T) {}

public:
    // virtual ~Storage() = default;
    virtual ~Storage() {
        for (const auto &kv: items) {
            unload_data(kv.second);
        }
    }

    // TODO: return tuple of files found and files successfully loaded
    virtual void load(
        const std::string &path,
        const std::string &extension
        // Does not process subdirs right now, as I have no idea how to
        // feature these without overriding files with name collisions.
    ) {
        FilePathList files = LoadDirectoryFilesEx(
            path.c_str(),
            extension.c_str(),
            false
        );

        for (auto current = 0ul; current < files.count; current++) {
            std::string name_key(GetFileNameWithoutExt(files.paths[current]));
            items[name_key] = load_data(files.paths[current]);
        }

        UnloadDirectoryFiles(files);
    }

    // Returns not actual content, but constant pointer to it, to refrain from
    // copying large data chunks around
    const T* operator[](const std::string& key) {
        // Using .at() and not [] there, because default implementation of []
        // makes it hard to debug things - instead of throwing an error on
        // non-existing value, it will return something weird
        return &items.at(key);
    }
};

class SpriteStorage : public Storage<Texture2D> {
protected:
    virtual Texture2D load_data(const std::string &path) override;
    virtual void unload_data(Texture2D data) override;
};

class SoundStorage : public Storage<Sound> {
protected:
    virtual Sound load_data(const std::string &path) override;
    virtual void unload_data(Sound data) override;
};

class MusicStorage : public Storage<Music> {
protected:
    virtual Music load_data(const std::string &path) override;
    virtual void unload_data(Music data) override;
};
