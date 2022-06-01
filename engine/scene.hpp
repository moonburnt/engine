#pragma once

#include "raylib.h"
#include <string>
#include <unordered_map>
#include <vector>

// Various scene management shenanigans

// Node is an abstract thing that can be attached to Scene or SceneManager
class Node {
protected:
    std::vector<Node*> children;
    Node* parent = nullptr;

    Vector2 pos = {0.0f, 0.0f};

public:
    Node() = default;
    virtual ~Node();

    // Get node's parent. If does not exist - returns nullptr.
    Node* get_parent();

    // Attach existing node as a child to this node
    void add_child(Node* node);

    // Create node of specified type and return pointer to it.
    // T must convert to Node*, else there will be dragons.
    template <typename T, typename... Args>
    T* create_child(Args&&... args) {
        children.push_back(new T(std::forward(args)...));
        return static_cast<T*>(children.back());
    }

    // Detach provided child from node.
    // If has not been attached - does nothing (for now)
    void remove_child(Node* node);

    // Attach this node to specific parent. If it has already been attached to
    // some, then detach it and move to new one, with all its ancestors
    void set_parent(Node* node);

    virtual void set_pos(Vector2 pos);

    // Get current node position in relevance to its parent
    virtual Vector2 get_pos();
    // Get absolute node position in the world.
    Vector2 get_abs_pos();

    // These arent pure-virtual for two reasons:
    // - Some children may not specify some of these. Say, audio node won't have
    // draw method.
    // - By default, these are used to cast something on node's children recursively
    virtual void update(float dt);

    virtual void draw();
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
    // Node storage.
    std::unordered_map<std::string, Node*> nodes;

    SceneManager();
    ~SceneManager();

    void set_current_scene(Scene* scene);
    void update(float dt);
    bool active;
    bool is_active();
};
