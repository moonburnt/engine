#pragma once

#include "raylib.h"
#include <string>
#include <unordered_map>
#include <vector>
#include "raybuff.hpp"
#include "spdlog/spdlog.h"
#include "formatters.hpp"

// #if defined(DRAW_DEBUG)
static constexpr Color DEBUG_DRAW_COLOR_FG = { 230, 41, 55, 155 };
static constexpr Color DEBUG_DRAW_COLOR_BG = { 255, 109, 194, 155 };
static constexpr Color DEBUG_DRAW_COLOR = { 230, 41, 55, 155 } ;
// #endif

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
private:
    // Scene, this node is attached too.
    // Should be nullptr for everything that isnt RootNode
    Scene* scene = nullptr;
    void attach_to_scene(Scene* _scene);

    // Check if node is scheduled to be deleted on the beginning of next update
    // cycle.
    // is_deleted() - public getter
    // mark_to_delete() - toggle this true
    bool _is_deleted = false;

    void detach(bool remove_from_parent);

    // Remove nullptr placeholders from storage
    void cleanup();

    void build_flat_children_vector(
        std::vector<Node*> &valid,
        std::vector<Node*> &to_remove
    );

    std::string tag = "Node";

    // Befriend with scene to allow it to set pointer to itself on root node's
    // init.
    friend class Scene;

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

    // Debug drawing method. Shouldn't be called directly, but from draw_recursive.
    // Won't do anything for base Node, thus protected and virtual.
    // DO NOT attempt to ifdef it there, coz if game and engine have different
    // compile-time flags, this will mess with vtable.
    // Rather than that, ifdef the logic inside overrides.
    // #if defined(DRAW_DEBUG)
    virtual void draw_debug();
    // #endif

public:
    virtual ~Node();

    Node() = default;
    Node(Align _align);

    // get offset coordinates of the specific side of the parent. Idk about naming
    // TODO: cache it for rectangle node, together with offset+world pos
    virtual Vector2 get_offset(Align) {
        return {0.0f, 0.0f};
    }

    void add_tag(const std::string &txt);

    std::string get_tag();

    // Get textual representation of a node. This may include whatever you want
    virtual std::string to_string() {
        return fmt::format("{} at {}", get_tag(), get_world_pos());
    }

    bool is_deleted();
    void mark_to_delete();

    // Get node's parent. If does not exist - returns nullptr.
    // TODO: maybe remove it, coz it messes with Scene
    Node* get_parent();

    // Get to the very beginning of branch, this node is attached to.
    // Returns the node itself if there is no parent.
    Node* get_root();

    // Get scene, this node's branch is attached to.
    // Returns nullptr if its detached.
    Scene* get_scene();

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
    void detach_child(Node* node);

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

    // For now, there is no point to make it virtual.
    // But maybe that will change later.
    void set_pos(Vector2 pos);

    // Get current node position in relevance to its parent
    Vector2 get_local_pos();
    // Get absolute node position in the world.
    Vector2 get_world_pos();

    // These arent pure-virtual, coz some children may not specify some of these.
    // Say, audio node won't have a draw method.
    // TODO: think if this should auto-update position of child nodes recursively.
    virtual void update(float dt);

    virtual void draw();

    // TODO: move this to be a templated in a delegated class
    virtual bool collides_with(Vector2 vec) {
        return (vec == get_world_pos());
    }

    virtual bool collides_with(Rectangle rec) {
        return CheckCollisionPointRec(get_world_pos(), rec);
    }
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

    void draw_debug() override;

public:
    // Don't overthink it for now
    // May optimize later if these will become an issue
    // RectangleNode(const Rectangle& rect);
    RectangleNode(Rectangle rect);

    Vector2 get_offset(Align side) override {
        Vector2 offset_value;

        switch (side) {
        case Align::TopLeft: {
            offset_value = {0.0f, 0.0f};
            break;
        }
        case Align::Top: {
            offset_value = {size.x / 2.0f, 0.0f};
            break;
        }
        case Align::TopRight: {
            offset_value = {size.x, 0.0f};
            break;
        }
        case Align::Left: {
            offset_value = {0.0f, size.y / 2.0f};
            break;
        }
        case Align::Center: {
            offset_value = {size.x / 2.0f, size.y / 2.0f};
            break;
        }
        case Align::Right: {
            offset_value = {size.x, size.y / 2.0f};
            break;
        }
        case Align::BottomLeft: {
            offset_value = {0.0f, size.y};
            break;
        }
        case Align::Bottom: {
            offset_value = {size.x / 2.0f, size.y};
            break;
        }
        case Align::BottomRight: {
            offset_value = {size.x, size.y};
            break;
        }
        }

        return offset_value;
    }

    Rectangle get_rect();

    // Check if node collides with other objects
    bool collides(RectangleNode other);
    bool collides(Rectangle _rect);
    bool collides(Vector2 _pos);
    // TODO: maybe create a class for circle
    // Then there could be relevant collision check function

    Rectangle get_collision_rect(RectangleNode other);
    Rectangle get_collision_rect(Rectangle _rect);

    // TEMPORARY. THIS SHOULD NOT BE EDITABLE.
    // TODO: DELET THIS, FIND SOME OTHER WORKAROUND
    void set_size(Vector2 s) {
        size = s;
    }

    bool collides_with(Vector2 vec) override {
        return CheckCollisionPointRec(vec, get_rect());
    }

    bool collides_with(Rectangle rec) override {
        return CheckCollisionPointRec(get_world_pos(), rec);
    }
};
