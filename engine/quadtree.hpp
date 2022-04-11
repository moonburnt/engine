#pragma once

#include "raylib.h"
#include <functional>
#include <vector>
// #include <array>
// For static assert's comparisons
#include <type_traits>

// An attempt to implement a QuadTree mechanism with raylib's primitives.
// Highly based on https://en.wikipedia.org/wiki/Quadtree#Pseudocode .
// Header-only because its designed as template.
// Should be usable both as a standalone solution and with ECS.

// TODO:
// - ability to set max depth (e.g max tree level)
// - ability to get nearest member

template <typename T> class QuadTree {
private:
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

    // Children.
    // Order may be changed - using not the one from wiki, but one used in math.
    // TODO: maybe move these to an array?
    QuadTree* north_east;
    QuadTree* north_west;
    QuadTree* south_west;
    QuadTree* south_east;

    QuadTree(size_t lvl, Rectangle _boundary, std::function<bool(T, Rectangle)> _contains)
        : boundary(_boundary)
        , level(lvl)
        , contains(_contains)
        , north_east(nullptr)
        , north_west(nullptr)
        , south_west(nullptr)
        , south_east(nullptr) {
        items.reserve(capacity);
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

        north_east = new QuadTree(
            new_level,
            {boundary.x + half_width, boundary.y, half_width, half_height},
            contains);
        north_west = new QuadTree(
            new_level,
            {boundary.x, boundary.y, half_width, half_height},
            contains);
        south_west = new QuadTree(
            new_level,
            {boundary.x, boundary.y + half_height, half_width, half_height},
            contains);
        south_east = new QuadTree(
            new_level,
            {boundary.x + half_width, boundary.x + half_height, half_width, half_height},
            contains);
    }

public:
    QuadTree(Rectangle _boundary, std::function<bool(T, Rectangle)> _contains)
        : boundary(_boundary)
        , level(0)
        , contains(_contains)
        , north_east(nullptr)
        , north_west(nullptr)
        , south_west(nullptr)
        , south_east(nullptr) {
        items.reserve(capacity);
    }

    ~QuadTree() {
        if (north_east != nullptr) delete north_east;
        if (north_west != nullptr) delete north_west;
        if (south_west != nullptr) delete south_west;
        if (south_east != nullptr) delete south_east;
    }

    // Insert specified point into quadtree. If invalid - returns false.
    bool insert(T p) {
        // Ignore items which do not belong to this tree's rect
        if (!contains(p, boundary)) {
            return false;
        }

        // If quadtree runs out of space - it gets divided. Thats why we check
        // if it has children and then if it ran out of space.
        // If necessary - subdivide it.
        if (items.size() < capacity && north_east == nullptr) {
            items.push_back(p);
            return true;
        }

        if (north_east == nullptr) {
            subdivide();
        }

        // If we got this far - we must already have children.
        // Thus trying to insert the point into one of them.
        if (north_east->insert(p)) return true;
        if (north_west->insert(p)) return true;
        if (south_west->insert(p)) return true;
        if (south_east->insert(p)) return true;

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
        if (north_east == nullptr) {
            return results;
        }

        // Else querrying results from children.
        // This may be inefficient and may need a rework. TODO
        std::vector<T> n_east = north_east->query_range(range);
        results.insert(results.end(), n_east.begin(), n_east.end());

        std::vector<T> n_west = north_west->query_range(range);
        results.insert(results.end(), n_west.begin(), n_west.end());

        std::vector<T> s_west = south_west->query_range(range);
        results.insert(results.end(), s_west.begin(), s_west.end());

        std::vector<T> s_east = south_east->query_range(range);
        results.insert(results.end(), s_east.begin(), s_east.end());

        return results;
    }

    // Remove elements from current level and its descendants.
    // TODO: maybe add counter to track amount of removed elements
    void clear() {
        items.clear();

        if (north_east != nullptr) {
            north_east->clear();
            north_west->clear();
            south_west->clear();
            south_east->clear();
        }
    }
};
