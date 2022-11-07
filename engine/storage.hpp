#pragma once

#include "raylib.h"

#include <string>
#include <unordered_map>
#include <functional>

// TODO: maybe rework Storage into subclassed unordered_map?
template <typename ContentType> class Storage {
protected:
    std::unordered_map<std::string, ContentType> items;

    virtual int load_with(
        const std::string &path,
        const std::string &extension,
        std::function<ContentType(const char*)> callback
    ) {
        FilePathList files = LoadDirectoryFilesEx(
            path.c_str(),
            extension.c_str(),
            false
        );

        for (auto current = 0ul; current < files.count; current++) {
            std::string name_key(GetFileNameWithoutExt(files.paths[current]));
            items[name_key] = callback(files.paths[current]);
        }

        int files_amount = files.count;

        UnloadDirectoryFiles(files);

        return files_amount;
    }

public:
    virtual ~Storage() = default;

    virtual void load(const std::string &path, const std::string &extension) = 0;

    // Returns not actual content, but constant pointer to it, to refrain from
    // copying large data chunks around
    const ContentType* operator[](const std::string& key) {
        // Using .at() and not [] there, because default implementation of []
        // makes it hard to debug things - instead of throwing an error on
        // non-existing value, it will return something weird
        return &items.at(key);
    }
};

class SpriteStorage : public Storage<Texture2D> {
public:
    void load(const std::string &path, const std::string &extension) override;
    ~SpriteStorage();
};

class SoundStorage : public Storage<Sound> {
public:
    void load(const std::string &path, const std::string &extension) override;
    ~SoundStorage();
};

class MusicStorage : public Storage<Music> {
public:
    void load(const std::string &path, const std::string &extension) override;
    ~MusicStorage();
};
