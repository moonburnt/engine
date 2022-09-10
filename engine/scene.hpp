#pragma once

#include "raylib.h"
#include <string>
#include <unordered_map>
#include <vector>

// Various scene management shenanigans

// Alignment for nodes
// Originally I've intended to implement AlignNode and set it exclusively for
// it and its relatives. But then I couldn't figure out how to check for
// parent's alignment. Thus this will be included in base node. For now.
enum class Align {
    TopLeft,
    Top,
    TopRight,
    Left,
    Center,
    Right,
    BottomLeft,
    Bottom,
    BottomRight
};

// Node is an abstract thing that can be attached to Scene or SceneManager
class Node {
protected:
    std::vector<Node*> children;
    Node* parent = nullptr;
    // Node alignment.
    // For now, does not affect basic nodes - just RectangleNode and its relatives
    Align align = Align::TopLeft;

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

    // Set node's alignment, which will affect placement of child nodes (qt-style)
    // TODO: think if apply_align() should be applied right away
    virtual void set_align(Align _align);
    Align get_align();
    // Manually re-calculate position of all child nodes according to current align
    // Not implemented, since for now we don't store abs_pos but use get_abs_pos()
    // to calculate it manually on demand.
    // void apply_align();

    virtual void set_pos(Vector2 pos);

    // Get current node position in relevance to its parent
    virtual Vector2 get_pos();
    // Get absolute node position in the world.
    Vector2 get_abs_pos();

    // These arent pure-virtual, coz some children may not specify some of these.
    // Say, audio node won't have a draw method.
    // TODO: think if this should auto-update position of child nodes recursively.
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
    // Don't overthink it for now
    // May optimize later if these will become an issue
    // RectangleNode(const Rectangle& rect);
    RectangleNode(Rectangle rect);

    virtual void set_pos(Vector2 pos) override;

    Rectangle get_rect();

    // Check if node collides with other objects
    bool collides(RectangleNode other);
    bool collides(Rectangle _rect);
    bool collides(Vector2 _pos);
    // TODO: maybe create a class for circle
    // Then there could be relevant collision check function

    Rectangle get_collision_rect(RectangleNode other);
    Rectangle get_collision_rect(Rectangle _rect);
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
