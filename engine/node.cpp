#include "node.hpp"
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

void Node::attach_to_scene(Scene* _scene) {
    // TODO: maybe ensure node can't be reattached? idk
    scene = _scene;
}

Node::Node(Align _align)
    : align(_align) {}

Node* Node::get_parent() {
    return parent;
}

Node* Node::get_root() {
    if (parent == nullptr) {
        return this;
    }
    else {
        return parent->get_root();
    }
}

Scene* Node::get_scene() {
    Node* root_node = get_root();
    if (root_node == nullptr) {
        return nullptr;
    }
    else {
        return root_node->scene;
    }
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
    // spdlog::info("updating node");
    update(dt);
    for (auto i: children) {
        // spdlog::info("now updating children");
        // It may be non-obvious, but Node will have access to private and
        // protected methods of other Node objects too.
        i->update_recursive(dt);
    }
}

// TODO: think if we should adjust base node's pos each frame
void Node::update(float) {
    // spdlog::info("a");
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
