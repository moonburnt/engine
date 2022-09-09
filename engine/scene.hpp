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

    void update_recursive(float dt);
    void draw_recursive();

public:
    virtual ~Node();

    // Get node's parent. If does not exist - returns nullptr.
    // TODO: maybe remove it, coz it messes with Scene
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

    // These arent pure-virtual, coz some children may not specify some of these.
    // Say, audio node won't have a draw method.
    virtual void update(float dt);

    virtual void draw();
};

// Node with rectangle attached to it.
// Should:
// - Be used as base for everything collideable (buttons, entities, etc)
// - Contain methods for collision checking
// TODO:
// - Maybe add ability to draw these, if compiled with dev flag and
// has debug config enabled
class RectangleNode: public Node {
protected:
    // Node's rectangle.
    // Without alignments, its top left will be equal pos.
    Rectangle rect = {0.0f, 0.0f, 0.0f, 0.0f};

public:
    RectangleNode(const Rectangle& rect);

    virtual void set_pos(Vector2 pos) override;

    Rectangle get_rect();
};

// Scene is a base for everything
class Scene {
private:
    // RootNode is a scene-exclusive thing that should serve as an entry point
    class RootNode: public Node {
        public:
            void update(float dt) override;
            void draw() override;
    };

    RootNode root;
    Color bg_color;

public:
    Scene(Color bg_color);
    Scene();

    virtual ~Scene() = default;

    void add_child(Node* node);

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
    Scene* current_scene = nullptr;

public:
    // Node storage. TODO: remove it, just like with Scene
    std::unordered_map<std::string, Node*> nodes;

    SceneManager();
    ~SceneManager();

    void set_current_scene(Scene* scene);
    void update(float dt);
    bool active;
    bool is_active();
};
