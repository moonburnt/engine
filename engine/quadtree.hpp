#pragma once

#include "raylib.h"
#include <functional>
#include <vector>

// An attempt to implement a QuadTree mechanism with raylib's primitives.
// Highly based on https://en.wikipedia.org/wiki/Quadtree#Pseudocode .
// Header-only because its designed as template.
// Should be usable both as a standalone solution and with ECS.

// TODO:
// - ability to set max depth (e.g max tree level)
// - ability to get nearest member

template <typename T> class QuadTree {
private:
    // Order is not the one from wiki, but one used in math.
    enum class TreeBranch {
        NorthEast,
        NorthWest,
        SouthWest,
        SouthEast
    };

    // Capacity of this tree node.
    const std::size_t capacity = 4;
    // Boundaries of this tree node.
    Rectangle boundary;
    // Depth level of this tree node. Increases with each generation by 1.
    size_t level;

    // Items in this tree node.
    // Can probably be implemented as array for better efficiency, but will do.
    std::vector<T> items;

    std::function<bool(T, Rectangle)> contains;

    // Children branches.
    std::vector<QuadTree> children;

    QuadTree(size_t lvl, Rectangle _boundary, std::function<bool(T, Rectangle)> _contains)
        : boundary(_boundary)
        , level(lvl)
        , contains(_contains) {
        items.reserve(capacity);
        children.reserve(4);
    }

    // Create 4 children that fully divide this quad into 4 quads of equal area.
    // Attempting to do it on node with pre-existing children will overwrite them
    // without freeing memory - proceed with caution!
    // TODO: maybe either add safety check
    void subdivide() {
        // Stub, may be incorrect
        float half_width = boundary.width / 2.0f;
        float half_height = boundary.height / 2.0f;

        size_t new_level = level + 1;

        children.push_back({
            new_level,
            {boundary.x + half_width, boundary.y, half_width, half_height},
            contains});
        children.push_back({
            new_level,
            {boundary.x, boundary.y, half_width, half_height},
            contains});
        children.push_back({
            new_level,
            {boundary.x, boundary.y + half_height, half_width, half_height},
            contains});
        children.push_back({
            new_level,
            {boundary.x + half_width, boundary.x + half_height, half_width, half_height},
            contains});
    }

public:
    QuadTree(Rectangle _boundary, std::function<bool(T, Rectangle)> _contains)
        : QuadTree(0, _boundary, _contains) {}

    virtual ~QuadTree() = default;

    // Insert specified point into quadtree. If invalid - returns false.
    bool insert(T p) {
        // Ignore items which do not belong to this tree's rect
        if (!contains(p, boundary)) {
            return false;
        }

        // If quadtree runs out of space - it gets divided. Thats why we check
        // if it has children and then if it ran out of space.
        // If necessary - subdivide it.
        if (items.size() < capacity && children.size() == 0) {
            items.push_back(p);
            return true;
        }

        if (children.size() == 0) {
            subdivide();
        }

        // If we got this far - we must already have children.
        // Thus trying to insert the point into one of them.
        if (children[static_cast<int>(TreeBranch::NorthEast)].insert(p)) {
            return true;
        }
        if (children[static_cast<int>(TreeBranch::NorthWest)].insert(p)) {
            return true;
        }
        if (children[static_cast<int>(TreeBranch::SouthWest)].insert(p)) {
            return true;
        }
        if (children[static_cast<int>(TreeBranch::SouthEast)].insert(p)) {
            return true;
        }

        // And this should never happen, but since non-void function must return
        // something at the end - returning false at the end.
        return false;
    }

    // Find and return all items within specified rect.
    std::vector<T> query_range(Rectangle range) {
        std::vector<T> results;

        // Abort if range is not within this quad's bounds.
        if (!CheckCollisionRecs(range, boundary)) {
            return results;
        }

        // Checking objects at this quad level.
        for (std::size_t i = 0; i < items.size(); i++) {
            if (contains(items[i], range)) {
                results.push_back(items[i]);
            }
        }

        // If there are no children - return results as is.
        if (children.size() == 0) {
            return results;
        }

        // Else querrying results from children.
        // This may be inefficient and may need a rework. TODO
        for (auto directon: children) {
            std::vector<T> dir_vec = direction.query_range(range);
            results.insert(results.end(), dir_vec.begin(), dir_vec.end());
        }

        return results;
    }

    // Remove elements from current level and its descendants.
    // TODO: maybe add counter to track amount of removed elements
    void clear() {
        items.clear();

        for (auto direction: children) {
            direction.clear();
        }
    }

    // Purge kids and clear content, returning tree to its default form
    void purge() {
        items.clear();

        children.clear();
    }
};
