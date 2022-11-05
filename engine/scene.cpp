#include "scene.hpp"
// To add vectors
#include "raybuff.hpp"
#include "spdlog/spdlog.h"
// For find()
#include <algorithm>

// Node
Node::~Node(){
    for (auto i: children) {
        delete i;
    }

    // TODO: maybe I should add code to also delete node from parent?
}

Node::Node(Align _align)
    : align(_align) {}

Node* Node::get_parent() {
    return parent;
}

void Node::add_child(Node* node) {
    // Node* node_parent = node->get_parent();
    // if (node_parent != nullptr) {
    //     node_parent->remove_child(node);
    // }
    // node->detach();
    // children.push_back(node);
    // node->parent = this;
    node->set_parent(this);
}

void Node::remove_child(Node* node) {
    std::vector<Node*>::iterator it;
    it = std::find(children.begin(), children.end(), node);

    // I think thats how it should work?
    if (it != children.end()) {
        children.erase(it);
    }
}

void Node::set_parent(Node* node) {
    // if (parent != nullptr) {
    //     parent->remove_child(this);
    // }
    if (parent == node) {
        return;
    }
    else {
        detach();
        // node->add_child(this);
        node->children.push_back(this);
        parent = node;
    }
}

void Node::detach() {
    if (parent == nullptr) {
        return;
    }
    else {
        parent->remove_child(this);
        parent = nullptr;
        set_dirty();
    }
}

void Node::set_align(Align _align) {
    if (align != _align) {
        align = _align;
        set_dirty();
    }
}

Align Node::get_align() {
    return align;
}

void Node::set_dirty() {
    pos_is_dirty = true;
    for (auto i: children) {
        i->set_dirty();
    }
}

void Node::calculate_world_pos() {
    if (parent != nullptr) {
        world_pos = parent->get_world_pos() + get_local_pos();
    }
    else {
        world_pos = get_local_pos();
    }
}

void Node::set_pos(Vector2 pos) {
    local_pos = pos;
    for (auto i: children) {
        i->set_dirty();
    }
}

Vector2 Node::get_local_pos() {
    return local_pos;
}

Vector2 Node::get_world_pos() {
    if (pos_is_dirty) {
        // TODO: think if this should be done recursively
        calculate_world_pos();
        pos_is_dirty = false;
    }

    return world_pos;
}

void Node::update_recursive(float dt) {
    // TODO: maybe move things around for draw cycle (what happens before and what
    // should happen after - children's or parent's things)
    spdlog::info("updating node");
    update(dt);
    for (auto i: children) {
        spdlog::info("now updating children");
        // It may be non-obvious, but Node will have access to private and
        // protected methods of other Node objects too.
        i->update_recursive(dt);
    }
}

// TODO: think if we should adjust base node's pos each frame
void Node::update(float) {
    spdlog::info("a");
}


void Node::draw() {}

#if defined(DRAW_DEBUG)
void Node::draw_debug() {}
#endif

void Node::draw_recursive() {
    #if defined(DRAW_DEBUG)
    draw_debug();
    #endif
    draw();
    // We may use different logic there. Or maybe even turn basic Node's logic
    // into a pure-virtual thing and write various implementations.
    // But for now it will do. TODO
    for (auto i: children) {
        // i->draw();
        // #if defined(DRAW_DEBUG)
        // i->draw_debug();
        // #endif
        i->draw_recursive();
    }
}

// RectangleNode
RectangleNode::RectangleNode(Rectangle rect)
    : size({rect.width, rect.height}) {
    // Idk if this is optimal resource-wise
    Node::set_pos({rect.x, rect.y});
}

void RectangleNode::calculate_world_pos() {
    if (parent != nullptr) {
        Vector2 origin_pos = parent->get_world_pos();
        // I think, alignment-related logic goes there?
        switch (parent->get_align()) {
        case Align::TopLeft: {
            // Do nothing since its the default align
            break;
        }
        // I think these are about right?
        case Align::Top: {
            origin_pos = {origin_pos.x - size.x / 2.0f, origin_pos.y};
            break;
        }
        case Align::TopRight: {
            origin_pos = {origin_pos.x - size.x, origin_pos.y};
            break;
        }
        case Align::Left: {
            origin_pos = {origin_pos.x, origin_pos.y - size.y / 2.0f};
            break;
        }
        case Align::Center: {
            origin_pos = {
                origin_pos.x - size.x / 2.0f,
                origin_pos.y - size.y / 2.0f
            };
            break;
        }
        case Align::Right: {
            origin_pos = {origin_pos.x - size.x, origin_pos.y - size.y / 2.0f};
            break;
        }
        case Align::BottomLeft: {
            origin_pos = {origin_pos.x, origin_pos.y - size.y};
            break;
        }
        case Align::Bottom: {
            origin_pos = {origin_pos.x - size.x / 2.0f, origin_pos.y - size.y};
            break;
        }
        case Align::BottomRight: {
            origin_pos = {origin_pos.x - size.x, origin_pos.y - size.y};
            break;
        }
        // Not adding default since we should cover all cases there
        }

        world_pos = origin_pos + get_local_pos();
    }
    else {
        world_pos = get_local_pos();
    }
}

Rectangle RectangleNode::get_rect() {
    // Coz it may be dirty
    Vector2 _wp = get_world_pos();
    return {_wp.x, _wp.y, size.x, size.y};
}

bool RectangleNode::collides(RectangleNode other) {
    return CheckCollisionRecs(get_rect(), other.get_rect());
}

bool RectangleNode::collides(Rectangle _rect) {
    return CheckCollisionRecs(get_rect(), _rect);
}

bool RectangleNode::collides(Vector2 _pos) {
    return CheckCollisionPointRec(_pos, get_rect());
}

Rectangle RectangleNode::get_collision_rect(RectangleNode other) {
    return GetCollisionRec(get_rect(), other.get_rect());
}

Rectangle RectangleNode::get_collision_rect(Rectangle _rect) {
    return GetCollisionRec(get_rect(), _rect);
}

#if defined(DRAW_DEBUG)
void RectangleNode::draw_debug() {
    DrawRectangleRec(get_rect(), DEBUG_DRAW_COLOR_FG);
    DrawRectangleLinesEx(get_rect(), 0.5f, DEBUG_DRAW_COLOR_BG);
}
#endif

// Scene
Scene::Scene(Color _bg_color)
    : bg_color(_bg_color) {}

void Scene::add_child(Node* node) {
    root.add_child(node);
}

void Scene::remove_child(Node* node) {
    root.remove_child(node);
}

void Scene::update(float) {}

void Scene::draw() {}

void Scene::update_recursive(float dt) {
    update(dt);
    root.update_recursive(dt);
}

void Scene::draw_recursive() {
    ClearBackground(bg_color);
    draw();
    root.draw_recursive();
}

// Scene manager

// There are two ways to work with scenes: to add scene manually each time,
// initializing it from zero and clearing up other scenes from memory.
// Or to keep all scenes initialized in some storage. For now, we are going for
// the first one, but this behavior may change in future.

bool SceneManager::try_to_switch_scene() {
    if (next_scene == nullptr) {
        return false;
    }

    if (current_scene != nullptr) {
        delete current_scene;
    };

    current_scene = next_scene;
    next_scene = nullptr;
    return true;
}

void SceneManager::set_current_scene(Scene* scene, bool ensure_unique) {
    if (ensure_unique) {
        if (current_scene == scene) {
            return;
        }
    }

    // Avoid scheduling a switch to the same scene multiple times
    if (next_scene == scene) {
        return;
    }
    // Ensure attempting to switch to different scenes within same update frame
    // won't cause now-invalid switch options to stay in memory
    else if (next_scene != nullptr) {
        delete next_scene;
    }

    next_scene = scene;

    // Old logic below
    // if (current_scene != nullptr) {
    //     delete current_scene;
    // };

    // current_scene = scene;
    // active = true;
}

void SceneManager::set_current_scene(Scene* scene) {
    set_current_scene(scene, false);
}

bool SceneManager::is_active() {
    return active;
}

void SceneManager::update(float dt) {
    if (!active) {
        return;
    }

    // Doing switch at the beginning and not end of update cycle, since otherwise
    // initial scene loading breaks.
    try_to_switch_scene();

    current_scene->update_recursive(dt);

    for (const auto& [_, i] : nodes) {
        // i->update(dt);
        i->update_recursive(dt);
    }

    BeginDrawing();
    current_scene->draw_recursive();

    for (const auto& [_, i] : nodes) {
        // i->draw();
        i->draw_recursive();
    }

    EndDrawing();
    // try_to_switch_scene();
}

SceneManager::~SceneManager() {
    if (current_scene != nullptr) {
        delete current_scene;
    }

    for (const auto &kv: nodes) {
        if (kv.second != nullptr) {
            delete kv.second;
        }
    }
}
