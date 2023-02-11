#pragma once

#include "node.hpp"
#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include "tasks.hpp"

// Various scene management shenanigans

// Scene is a base for everything
class Scene {
private:
    // Root node that should serve as an entry point.
    Node root;
    Color bg_color = {0, 0, 0, 0};

    // Task manager for node tasks.
    // Add, remove, etc
    TaskManager node_mgr;

    // Allow LayerStorage to access our private and protected things
    friend class LayerStorage;

    // Flat list of nodes to update this frame
    std::vector<Node*> children_nodes;

    std::string tag = "";

protected:
    void update_recursive(float dt);
    void draw_recursive();

public:
    Scene(Color bg_color);
    Scene();

    // virtual ~Scene() = default;
    virtual ~Scene();

    void add_tag(const std::string &txt);
    std::string get_tag();

    void add_child(Node* node);

    const std::vector<Node*>& get_children() {
        return root.children;
    }

    template <typename T, typename... Args>
    T* create_child(Args&&... args) {
        return root.create_child<T>(std::forward(args)...);
    }

    void detach_child(Node* node);

    virtual void update(float dt);
    virtual void draw();
};

class LayerStorage {
private:
    Scene* current_scene = nullptr;
    Scene* next_scene = nullptr;

    friend class SceneManager;

protected:
    // Switch scene to the next scene, if next_scene != nullptr
    // Implemented coz using set_current_scene() to perform actual switch
    // mid update cycle caused segfaults
    bool try_to_switch();

public:
    ~LayerStorage();

    // Schedule provided scene to be set as current_scene at the beginning of
    // the next update cycle. If ensure_unique is set - will also check if new
    // scene is not the same as current scene. Default - false.
    // TODO: consider a less deceiving name for this
    void set_current(Scene* scene, bool ensure_unique);
    void set_current(Scene* scene);

    // Get current scene.
    // Returns nullptr if there is none.
    Scene* get_current();

    Scene* get_future();

    // Get current scene is there is one, future if its nullptr, else nullptr
    Scene* get_current_or_future();

    void update(float dt);
    void draw();
};

class SceneManager {
private:
    std::map<const std::string, LayerStorage> layers = {};

    void try_to_switch_layers();

public:
    // Node storage. TODO: remove it, just like with Scene
    // std::unordered_map<std::string, Node*> nodes;

    ~SceneManager();

    // I dont think we will need to remove layers, so there is no deleter
    LayerStorage* configure_layer(const std::string& layer_name, Scene* scene) {
        layers[layer_name].set_current(scene);

        return &layers.at(layer_name);
    }

    LayerStorage* configure_layer(const std::string& layer_name) {
        Scene* s = new Scene();
        s->add_tag(layer_name);
        return configure_layer(layer_name, s);
    }

    LayerStorage* get_layer(const std::string& layer_name) {
        return &layers.at(layer_name);
    }

    void update(float dt);
    bool active = true;
    bool is_active();
};
