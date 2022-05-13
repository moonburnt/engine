#pragma once

#include <raylib.h>

#include <string>
#include <unordered_map>

// TODO: maybe rework Storage into subclassed unordered_map?
template <typename ContentType> class Storage {
protected:
    std::unordered_map<std::string, ContentType> items;

public:
    virtual ~Storage() = default;

    virtual void load(std::string path, std::string extension) = 0;

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
    void load(std::string path, std::string extension) override;
    ~SpriteStorage();
};

class SoundStorage : public Storage<Sound> {
public:
    void load(std::string path, std::string extension) override;
    ~SoundStorage();
};

class MusicStorage : public Storage<Music> {
public:
    void load(std::string path, std::string extension) override;
    ~MusicStorage();
};
