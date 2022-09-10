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

Node* Node::get_parent() {
    return parent;
}

void Node::add_child(Node* node) {
    Node* node_parent = node->get_parent();
    if (node_parent != nullptr) {
        node_parent->remove_child(node);
    }

    children.push_back(node);
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
    if (parent != nullptr) {
        parent->remove_child(this);
    }

    node->add_child(this);
}

void Node::set_align(Align _align) {
    align = _align;
}

Align Node::get_align() {
    return align;
}

void Node::set_pos(Vector2 _pos) {
    pos = _pos;
}

Vector2 Node::get_pos() {
    return pos;
}

Vector2 Node::get_abs_pos() {
    if (parent != nullptr) {
        // This may need some optimisations, too much Vector copying. TODO
        return parent->get_abs_pos() + get_pos();
    }
    else {
        return get_pos();
    }
}

void Node::update_recursive(float dt) {
    for (auto i: children) {
        i->update(dt);
        // It may be non-obvious, but Node will have access to private and
        // protected methods of other Node objects too.
        i->update_recursive(dt);
    }
}

// TODO: think if we should adjust base node's pos each frame
void Node::update(float) {}


void Node::draw() {}

#if defined(DRAW_DEBUG)
void Node::draw_debug() {}
#endif

void Node::draw_recursive() {
    // We may use different logic there. Or maybe even turn basic Node's logic
    // into a pure-virtual thing and write various implementations.
    // But for now it will do. TODO
    for (auto i: children) {
        i->draw();
        #if defined(DRAW_DEBUG)
        i->draw_debug();
        #endif
        i->draw_recursive();
    }
}

// RectangleNode
RectangleNode::RectangleNode(Rectangle _rect)
    : rect(_rect) {
    // Idk if this is optimal resource-wise
    Node::set_pos({rect.x, rect.y});
}

void RectangleNode::set_pos(Vector2 _pos) {
    Node::set_pos(_pos);

    // TODO: alignment logic goes there
    rect.x = pos.x;
    rect.y = pos.y;
}

Vector2 RectangleNode::get_abs_pos() {
    // Maybe I should call Node's method inside and put custom logic afterwards?
    // Idk, will do for now.

    if (parent != nullptr) {
        Vector2 origin_pos = parent->get_abs_pos();
        // I think, alignment-related logic goes there?
        switch (parent->get_align()) {
        case Align::TopLeft: {
            // Do nothing since its the default align
            break;
        }
        // I think these are about right?
        case Align::Top: {
            origin_pos = {origin_pos.x - rect.width / 2.0f, origin_pos.y};
            break;
        }
        case Align::TopRight: {
            origin_pos = {origin_pos.x - rect.width, origin_pos.y};
            break;
        }
        case Align::Left: {
            origin_pos = {origin_pos.x, origin_pos.y - rect.height / 2.0f};
            break;
        }
        case Align::Center: {
            origin_pos = {
                origin_pos.x - rect.width / 2.0f,
                origin_pos.y - rect.height / 2.0f
            };
            break;
        }
        case Align::Right: {
            origin_pos = {origin_pos.x - rect.width, pos.y - rect.height / 2.0f};
            break;
        }
        case Align::BottomLeft: {
            origin_pos = {origin_pos.x, origin_pos.y - rect.height};
            break;
        }
        case Align::Bottom: {
            origin_pos = {origin_pos.x - rect.width / 2.0f, origin_pos.y - rect.height};
            break;
        }
        case Align::BottomRight: {
            origin_pos = {origin_pos.x - rect.width, origin_pos.y - rect.height};
            break;
        }
        // Not adding default since we should cover all cases there
        }

        return origin_pos + get_pos();
    }
    else {
        return get_pos();
    }
}

Rectangle RectangleNode::get_rect() {
    return rect;
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
    // DrawRectangle
    // TODO: do something about rect's first two values being off
    // TODO: switch to DrawRectangleRec
    Vector2 abs_pos = get_abs_pos();
    DrawRectangleV(abs_pos, {rect.width, rect.height}, DEBUG_DRAW_COLOR_BG);
    DrawRectangleLines(
        abs_pos.x, abs_pos.y, rect.width, rect.height, DEBUG_DRAW_COLOR_FG);
}
#endif

// RootNode
// Thats how we specify logic of classes described inside other classes
void Scene::RootNode::update(float dt) {
    update_recursive(dt);
}

void Scene::RootNode::draw() {
    draw_recursive();
}


// Scene
Scene::Scene(Color _bg_color)
    : bg_color(_bg_color) {
}

Scene::Scene()
    : Scene({245, 245, 245, 255}) {
}

void Scene::add_child(Node* node) {
    root.add_child(node);
}

void Scene::remove_child(Node* node) {
    root.remove_child(node);
}

void Scene::update(float dt) {
    root.update(dt);
}

void Scene::draw() {
    ClearBackground(bg_color);
    root.draw();
}

// Scene manager

// There are two ways to work with scenes: to add scene manually each time,
// initializing it from zero and clearing up other scenes from memory.
// Or to keep all scenes initialized in some storage. For now, we are going for
// the first one, but this behavior may change in future.
void SceneManager::set_current_scene(Scene* scene) {
    if (current_scene != nullptr) {
        delete current_scene;
    };

    current_scene = scene;
    // active = true;
}

bool SceneManager::is_active() {
    return active;
}

void SceneManager::update(float dt) {
    if (!active) {
        return;
    }

    current_scene->update(dt);

    for (const auto& [_, i] : nodes) {
        i->update(dt);
    }

    BeginDrawing();
    current_scene->draw();

    for (const auto& [_, i] : nodes) {
        i->draw();
    }

    EndDrawing();
}

// Default SceneManager's constructor is all way down, coz TitleScreen is in its
// body. But don't worry - even if instantiation is declared above, nothing bad
// will happen - this one will get triggered correctly
SceneManager::SceneManager()
    : active(true) {
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
