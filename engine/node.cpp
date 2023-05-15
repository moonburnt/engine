#include "node.hpp"
// To add vectors
#include "raybuff.hpp"
#include "spdlog/spdlog.h"
// For find()
#include <algorithm>

// Node
Node::~Node(){
    spdlog::debug("Deleting node {}", tag);
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

void Node::detach(bool remove_from_parent) {
    if (parent == nullptr) {
        spdlog::info("Node {}'s parent is null", tag);
        return;
    }
    else {
        if (remove_from_parent) {
            parent->detach_child(this);
        }
        parent = nullptr;
        set_dirty();
    }
}

void Node::cleanup() {
    children.erase(
        std::remove_if(
            children.begin(),
            children.end(),
            [](Node* i){ return i == nullptr; }),
        children.end()
    );
}

void Node::build_flat_children_vector(
    std::vector<Node*> &valid,
    std::vector<Node*> &to_remove
) {
    if (is_deleted()) {
        for (auto i: children) {
            if (i != nullptr) {
                // spdlog::info("c {}", i->get_tag());
                i->mark_to_delete();
                i->build_flat_children_vector(valid, to_remove);
            }
        }
        // children.clear();
        cleanup();
        detach();
        // spdlog::info("pb {}", tag);
        to_remove.push_back(this);
        // spdlog::info("{}", store.size());
    }
    else {
        // Old update_recursive logic was "first we update parent, then
        // children". Lets follow it for now
        valid.push_back(this);
        for (auto i: children) {
            if (i != nullptr) {
                i->build_flat_children_vector(valid, to_remove);
            }
        }
        cleanup();
    }
}

void Node::add_tag(const std::string &txt) {
    tag = txt;
}

std::string Node::get_tag() {
    return tag;
}

bool Node::is_deleted() {
    return _is_deleted;
}

void Node::mark_to_delete() {
    _is_deleted = true;
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
    node->set_parent(this);
}

// This won't delete child nodes, coz that caused too much headache.
// This will also show an incorrect child counter, coz it replaces these with nullptr
// Thus never trust the child counter, filter it manually from nullptr
void Node::detach_child(Node* node) {
    node->detach(false);

    spdlog::info("detaching child {}, current len {}", node->get_tag(), children.size());
    std::vector<Node*>::iterator it;
    it = std::find(children.begin(), children.end(), node);

    // I think thats how it should work?
    if (it != children.end()) {
        // If we will remove node there, it will cause iteration issues.
        // For that reason, lets try to replace it with nullptr, patch
        // update_recursive() and draw_recursive() to support that and do
        // something about it later. TODO
        // children.erase(it);

        // Node* ch = children[std::distance(children.begin(), it)];
        children[std::distance(children.begin(), it)] = nullptr;

        // // Remove tangling nodes
        // if (ch->is_deleted()) {
        //     delete ch;
        // }
    }

    // TODO: implement cleanup logic to purge all nullptr nodes from there.

    // spdlog::info("new size {}", children.size());
}

void Node::set_parent(Node* node) {
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
    detach(true);
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

// TODO: think if we should adjust base node's pos each frame
void Node::update(float) {
}


void Node::draw() {}

void Node::draw_debug() {}

// RectangleNode
RectangleNode::RectangleNode(Rectangle rect)
    : size({rect.width, rect.height}) {
    // Idk if this is optimal resource-wise
    Node::set_pos({rect.x, rect.y});
}

void RectangleNode::calculate_world_pos() {
    if (parent != nullptr) {
        // Vector2 origin_pos = parent->get_world_pos();
        Vector2 origin_pos = parent->get_world_pos()+parent->get_offset(align);
        // switch (parent->get_align()) {
        switch (align) {
        case Align::TopLeft: {
            // Do nothing since its the default align
            break;
        }
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

void RectangleNode::draw_debug() {
    #if defined(DRAW_DEBUG)
    DrawRectangleRec(get_rect(), DEBUG_DRAW_COLOR_FG);
    DrawRectangleLinesEx(get_rect(), 0.5f, DEBUG_DRAW_COLOR_BG);
    #endif
}
