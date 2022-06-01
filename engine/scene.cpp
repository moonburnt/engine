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

void Node::update(float dt) {
    for (auto i: children) {
        i->update(dt);
    }
}

void Node::draw() {
    for (auto i: children) {
        i->draw();
    }
}

// Scene
Scene::Scene(Color _bg_color)
    : bg_color(_bg_color) {
}

Scene::Scene()
    : bg_color({245, 245, 245, 255}) {
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
    ClearBackground(current_scene->bg_color);
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
