#pragma once

#include "raylib.h"
#include "sound.hpp"
#include <string>
#include <unordered_map>

// Node is an abstract thing that can be attached to Scene or SceneManager
class Node {
public:
    virtual ~Node() = default;

    virtual void update() = 0;
    virtual void draw() = 0;
};

// Scene is an abstract class that can't be instantiated directly, but can be
// subclassed. This is how we do ABC interfaces in c++
// In this case its located in header, coz SceneManager needs it
class Scene {
public:
    std::unordered_map<std::string, Node*> nodes;

    // Thats how we define abstract functions
    virtual void update(float dt) = 0;
    virtual void draw() = 0;

    // This is a scene's destructor. Which can be overriden, but not necessary.
    virtual ~Scene() = default;

    Color bg_color;

    Scene();
    Scene(Color bg_color);
};

class SceneManager {
private:
    // We are using pointer to Scene, to make it work with Scene's children.
    // By default its set to nullptr, to verify if children has been configured.
    Scene* current_scene = nullptr;

public:
    // I don't think it should be there, but will do for now
    // TODO: move it from scene manager's update cycle to GameWindow's run()
    // (and into separate thread)
    MusicManager music_mgr;
    // Node storage.
    std::unordered_map<std::string, Node*> nodes;

    SceneManager();
    ~SceneManager();

    void set_current_scene(Scene* scene);
    void run_update_loop();
    bool active;
    bool is_active();
};

class GameWindow {
protected:
    bool initialized;

public:
    SceneManager sc_mgr;

    GameWindow();

    // Initialize game window
    void init(int x, int y, std::string title, int fps);
    void init(int x, int y, std::string title);
    void init(int x, int y);
    void init();

    // Run scene manager and other stuff
    void run();
};
