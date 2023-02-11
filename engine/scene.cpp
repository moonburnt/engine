#include "scene.hpp"
// To add vectors
#include "raybuff.hpp"
#include "spdlog/spdlog.h"

// Scene
Scene::Scene() {
    root.attach_to_scene(this);
}

Scene::~Scene() {
    spdlog::debug("Deleting scene");
}

Scene::Scene(Color _bg_color)
    : bg_color(_bg_color) {
    root.attach_to_scene(this);
}

void Scene::add_child(Node* node) {
    root.add_child(node);
}

void Scene::detach_child(Node* node) {
    root.detach_child(node);
}

void Scene::update(float) {}

void Scene::draw() {}

void Scene::update_recursive(float dt) {
    // Cleanup previous scene's children nodes.
    children_nodes.clear();

    // Delete nodes scheduled for removal on previous frame.
    std::vector<Node*> to_remove;
    root.build_flat_children_vector(children_nodes, to_remove);

    for (auto i = 0ul; i < to_remove.size(); i++) {
        Node* n = to_remove.at(i);
        spdlog::info("rm {}", n->get_tag());
        to_remove.at(i) = nullptr;
        delete n;
    }
    to_remove.clear();

    // TODO: maybe build a single list there, without needs to go for that
    // recursive thingy few times? Or few lists, but at once
    node_mgr.perform_tasks();

    update(dt);
    for (auto i: children_nodes) {
        i->update(dt);
    }
}

void Scene::draw_recursive() {
    ClearBackground(bg_color);
    draw();
    for (auto i: children_nodes) {
        #if defined(DRAW_DEBUG)
        i->draw_debug();
        #endif
        i->draw();
    }
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

Scene* SceneManager::get_current_scene() {
    return current_scene;
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

    // TODO: drop this stuff in favor of multi-scene setups
    for (const auto& [_, i] : nodes) {
        // i->update(dt);
        i->update(dt);
    }

    BeginDrawing();
    current_scene->draw_recursive();

    for (const auto& [_, i] : nodes) {
        // i->draw();
        i->draw();
    }

    EndDrawing();
    // try_to_switch_scene();
}

SceneManager::~SceneManager() {
    spdlog::debug("Deleting scene manager");
    if (current_scene != nullptr) {
        delete current_scene;
    }

    for (const auto &kv: nodes) {
        if (kv.second != nullptr) {
            delete kv.second;
        }
    }
}
