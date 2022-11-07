#pragma once

#include "raylib.h"
#include <string>
#include <unordered_map>
#include <vector>

#if defined(DRAW_DEBUG)
static constexpr Color DEBUG_DRAW_COLOR_FG = RED;
static constexpr Color DEBUG_DRAW_COLOR_BG = PINK;
static constexpr Color DEBUG_DRAW_COLOR = RED;
#endif

// Node stuff.

// Forward declaration to make node compile
class Scene;

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

    // local_pos is position relative to parent
    // used to calculate children's world pos
    Vector2 local_pos = {0.0f, 0.0f};
    // world_pos is global position in the world
    // used to draw things and perform all calculations
    Vector2 world_pos = {0.0f, 0.0f};
    // If this one is set to true - cached world_pos will be recalculated
    bool pos_is_dirty = true;
    // Set children's pos dirty recursively
    void set_dirty();
    // Calculate world pos and update world_pos
    // virtual coz logic may be altered depending on node
    virtual void calculate_world_pos();

    // Attach debug drawing method if compiled with DRAW_DEBUG flag
    // It shouldn't be called directly, but from draw_recursive.
    // Won't do anything for base Node, thus protected and virtual
    #if defined(DRAW_DEBUG)
    virtual void draw_debug();
    #endif

public:
    virtual ~Node();

    Node() = default;
    Node(Align _align);

    // Get node's parent. If does not exist - returns nullptr.
    // TODO: maybe remove it, coz it messes with Scene
    Node* get_parent();

    // Get to the very beginning of branch, this node is attached to.
    // Returns the node itself if there is no parent.
    Node* get_root();

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

    // Detach node from current parent (if exists)
    void detach();

    // Set node's alignment, which will affect placement of child nodes (qt-style)
    // TODO: think if apply_align() should be applied right away
    // virtual void set_align(Align _align);
    void set_align(Align _align);
    Align get_align();
    // Manually re-calculate position of all child nodes according to current align
    // Not implemented yet - tempting.
    // void apply_align();

    virtual void set_pos(Vector2 pos);

    // Get current node position in relevance to its parent
    Vector2 get_local_pos();
    // Get absolute node position in the world.
    Vector2 get_world_pos();

    // TODO: consider moving this to protected, since they aren't intended to
    // be called directly.
    void update_recursive(float dt);
    void draw_recursive();

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
class RectangleNode: public Node {
protected:
    // Node rectangle's width and height
    // Rect itself will be constructed on get_rect() from
    // get_world_pos() and size.
    // TODO: consider caching this.
    Vector2 size = {0.0f, 0.0f};

    void calculate_world_pos() override;

    #if defined(DRAW_DEBUG)
    void draw_debug() override;
    #endif

public:
    // Don't overthink it for now
    // May optimize later if these will become an issue
    // RectangleNode(const Rectangle& rect);
    RectangleNode(Rectangle rect);

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
