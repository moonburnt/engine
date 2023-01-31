#pragma once

#include "node.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include "tasks.hpp"

// Various scene management shenanigans

// Scene is a base for everything
class Scene {
private:
    // Root node that should serve as an entry point.
    Node root;
    Color bg_color = {245, 245, 245, 255};

    // Task manager for node tasks.
    // Add, remove, etc
    TaskManager node_mgr;

    // Allow SceneManager to access our private and protected things
    friend class SceneManager;

protected:
    void update_recursive(float dt);
    void draw_recursive();

public:
    Scene(Color bg_color);
    Scene();

    // virtual ~Scene() = default;
    virtual ~Scene();

    void add_child(Node* node);

    const std::vector<Node*>& get_children() {
        return root.children;
    }

    template <typename T, typename... Args>
    T* create_child(Args&&... args) {
        return root.create_child<T>(std::forward(args)...);
    }

    void remove_child(Node* node);

    virtual void update(float dt);
    virtual void draw();
};

class SceneManager {
private:
    // We are using pointer to Scene, to make it work with Scene's children.
    // By default its set to nullptr, to verify if children has been configured.
    // TODO: consider replacing this logic with deque to simplify checks in
    // try_to_switch_scene call
    Scene* current_scene = nullptr;
    Scene* next_scene = nullptr;

protected:
    // Switch scene to the next scene, if next_scene != nullptr
    // Implemented coz using set_current_scene() to perform actual switch
    // mid update cycle caused segfaults
    bool try_to_switch_scene();

public:
    // Node storage. TODO: remove it, just like with Scene
    std::unordered_map<std::string, Node*> nodes;

    ~SceneManager();

    // Schedule provided scene to be set as current_scene at the beginning of
    // the next update cycle. If ensure_unique is set - will also check if new
    // scene is not the same as current scene. Default - false.
    // TODO: consider a less deceiving name for this
    void set_current_scene(Scene* scene, bool ensure_unique);
    void set_current_scene(Scene* scene);

    // Get current scene.
    // Returns nullptr if there is none.
    Scene* get_current_scene();

    void update(float dt);
    bool active = true;
    bool is_active();
};
