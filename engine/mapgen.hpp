#pragma once

#include <raylib.h>
#include <optional>
#include <unordered_map>
#include <vector>
#include <functional>

// Tiled map generator

// TODO: maybe rename point to something else?
struct Point {
    int x;
    int y;
};

// TODO: Maybe implement different types of tiled map?
// E.g: TiledMapBase, TiledMapStatic (the same as TiledMap but you can't move or
// delete objects, just these are stored directly on grid). And probably a
// container to store few maps inside and process their content at once.

template<typename T> class TiledMap {
private:
    Point map_size;
    Point tile_size;
    size_t grid_size;
    Vector2 map_real_size;

    std::unordered_map<int, T> map_objects;
    size_t map_objects_amount;

    // ID of item used as failsafe when tile is empty. Default = -1.
    int placeholder_id;
    // Toggle that returns object from map
    bool return_placeholder;

    std::vector<int> grid;

    // Callback function to draw objects
    std::function<void(T, Vector2)> draw_callback;

public:
    TiledMap(Point _map_size, Point _tile_size, std::function<void(T, Vector2)> _draw_callback)
        : map_size(_map_size)
        , tile_size(_tile_size)
        , grid_size(map_size.x * map_size.y)
        , map_real_size(
            {static_cast<float>(map_size.x * tile_size.x),
            static_cast<float>(map_size.y * tile_size.y)})
        , map_objects_amount(0)
        , placeholder_id(-1)
        , return_placeholder(false)
        , grid(grid_size, placeholder_id)
        , draw_callback(_draw_callback) {
    }

    // Set placeholder tile an its return policy.
    void set_placeholder(int _placeholder_id, bool _return_placeholder) {
        placeholder_id = _placeholder_id;
        return_placeholder = _return_placeholder;
    }

    // Add new object to map_objects storage. Returns object id
    int add_object(T object) {
        map_objects[map_objects_amount] = object;
        map_objects_amount++;

        return map_objects_amount-1;
    }

    // Replace object id in grid[grid_index] with placeholder id.
    // If delete_from_storage is set to true - also delete from map_objects.
    // This is a thing, coz same object may appear on multiple tiles - floors, etc.
    void clear_tile(int grid_index, bool delete_from_storage) {
        // TODO: maybe add safety checks to ensure requested object exists in storage?
        if (delete_from_storage) {
            map_objects.erase(grid[grid_index]);
        }
        grid[grid_index] = placeholder_id;
    }

    // Place specific object from storage on specified space, without removing it
    // from any other place. May be used to spawn items and to place items that
    // may exist on multiple tiles at once, such as floor.
    // If tile already has item on it thats not placeholder - wont do anything.
    // Returns result of procedure.
    bool place_object(size_t grid_index, int object_id) {
        if (grid[grid_index] == placeholder_id) {
            grid[grid_index] = object_id;
            return true;
        }
        return false;
    }

    // Place object from storage on specified space or replace object already
    // occupying that slot. If remove_from_storage is set to true - will also
    // remove object pre-existing on that tile from storage
    void place_or_replace(size_t grid_index, int object_id, bool delete_from_storage) {
        if (grid[grid_index] != placeholder_id) {
            clear_tile(grid_index, delete_from_storage);
        }

        grid[grid_index] = object_id;
    }

    // Move object from grid[grid_index] to grid[new_grid_index].
    // If there is an other object - switch places with it.
    void move_object(int grid_index, int new_grid_index) {
        int first_id = grid[grid_index];
        int second_id = grid[new_grid_index];

        // I think this will do?
        place_or_replace(new_grid_index, first_id, false);
        place_or_replace(grid_index, second_id, false);
    }

    // const T* get_tile_content(size_t grid_index);
    // Get pointer to object with specified id
    const T* get_object_by_id(int object_id) {
        return &map_objects.at(object_id);
    }

    // Get pointer to object from specified tile.
    // If return_placeholder is set to false and specified tile contains placeholder
    // tile - will return nullptr.
    const T* get_object_from_grid(size_t grid_index) {
        int object_id = grid[grid_index];

        if (object_id == placeholder_id && !return_placeholder) {
            return nullptr;
        }

        return get_object_by_id(object_id);
    }

    // Get first tile that contains object with specified id, or std::nullopt
    std::optional<Point> find_object_tile(int object_id) {
        for (auto index = 0u; index < grid_size; index++) {
            if (grid[index] == object_id) {
                return index_to_tile(index);
            }
        }

        return std::nullopt;
    }

    // Returns true if requrested things are within map's borders, false otherwise
    bool is_vec_on_map(Vector2 vec) {
        return (
            (0 <= vec.x) && (vec.x < map_real_size.x) && (0 <= vec.y) &&
            (vec.y < map_real_size.y));
    }

    bool is_index_on_map(size_t grid_index) {
        return ((0 <= grid_index) && (grid_index < grid_size));
    }

    bool is_tile_on_map(Point tile) {
        return (
            (0 <= tile.x) && (tile.x < map_size.x) && (0 <= tile.y) && (tile.y < map_size.y));
    }

    // Attempting to use out-of-bounds values on these will cause issues.
    // Thus don't forget to call is_<type>_on_map before using them!
    Point index_to_tile(size_t index) {
        int y = index / map_size.x;
        int x = index % map_size.x;
        return Point{x, y};
    }

    size_t tile_to_index(Point pos) {
        return static_cast<size_t>(pos.y * map_size.x + pos.x);
    }

    Vector2 index_to_vec(size_t index) {
        auto tile = index_to_tile(index);
        return Vector2{
            static_cast<float>(tile.x * tile_size.x),
            static_cast<float>(tile.y * tile_size.y)};
    }

    Point vec_to_tile(Vector2 vec) {
        return Point{
            static_cast<int>(vec.x) / tile_size.x,
            static_cast<int>(vec.y) / tile_size.y};
    }

    Vector2 tile_to_vec(Point tile) {
        return Vector2{
            static_cast<float>(tile.x * tile_size.x),
            static_cast<float>(tile.y * tile_size.y)};
    }

    size_t vec_to_index(Vector2 vec) {
        int x = vec.x / tile_size.x;
        int y = vec.y / tile_size.y;
        return static_cast<size_t>(y * map_size.x + x);
    }

    // Getters for various map-related stats
    Point get_tile_size() {
        return tile_size;
    }
    Point get_map_size() {
        return map_size;
    }
    size_t get_grid_size() {
        return grid_size;
    }
    // Returns entity ids of each object on map.
    std::vector<int> get_map_layout() {
        return grid;
    }

    void draw() {
        for (size_t i = 0; i < grid_size; i++) {
            draw_callback(map_objects[grid[i]], index_to_vec(i));
        }
    }
};

// Class that generates tiled map based on provided color-action pair.
// Well, kinda. You have to bring initialized map of T type to generate().
// Because generator does not know which arguments your T may need to initialize.
// I planned to hardcode it to TiledMap's api, but then found it to be sadge.
template<typename T> class ColorGen {
protected:
    std::unordered_map<int, std::function<void(T&, size_t)>> callbacks;
    std::vector<int> grid;
    Point map_size;

public:
    ColorGen() {}

    ColorGen(Image map_file) {
        prepare(map_file);
    }

    Point get_size() {
        return map_size;
    }

    void add_relationship(Color color, std::function<void(T&, size_t)> callback) {
        callbacks[ColorToInt(color)] = callback;
    }

    void prepare(Image map_file) {
        // Generally each generator instance must be tied to one map file.
        // But if its not the case - clearing up current grid.
        grid.clear();

        map_size = {map_file.width, map_file.height};

        grid.reserve(map_size.x * map_size.y);

        for (auto current_y = 0; current_y < map_size.y; current_y++) {
            for (auto current_x = 0; current_x < map_size.x; current_x++) {
                grid.push_back(ColorToInt(GetImageColor(map_file, current_x, current_y)));
            }
        }
    }

    void generate(T& map) {
        if (grid.size() == 0) {
            return;
        }

        for (size_t grid_index = 0; grid_index < grid.size(); grid_index++) {
            int color = grid[grid_index];
            if (callbacks.count(color) > 0) {
                callbacks[color](map, grid_index);
            }
        }
    }

};

// Deep tiled map from Fortune Crawler
template<typename T> class TiledMapDeep {
private:
    Point map_size;
    Point tile_size;
    size_t grid_size;
    Vector2 map_real_size;

    std::unordered_map<int, T> map_objects;
    size_t map_objects_amount;

    // std::vector<int> grid;
    std::vector<std::vector<int>> grid;

    // Callback function to draw objects
    std::function<void(T, Vector2)> draw_callback;

public:
    TiledMapDeep(Point _map_size, Point _tile_size, std::function<void(T, Vector2)> _draw_callback)
        : map_size(_map_size)
        , tile_size(_tile_size)
        , grid_size(map_size.x * map_size.y)
        , map_real_size(
            {static_cast<float>(map_size.x * tile_size.x),
            static_cast<float>(map_size.y * tile_size.y)})
        , map_objects_amount(0)
        , grid(grid_size) // I hope this will work, coz grid(grid_size, {}) didn't
        , draw_callback(_draw_callback) {
    }

    // Add new object to map_objects storage. Returns object id
    int add_object(T object) {
        map_objects[map_objects_amount] = object;
        map_objects_amount++;

        return map_objects_amount-1;
    }

    // Clear provided tile from everything. If bool set to true - also purge objects
    void clear_tile(int grid_index, bool delete_from_storage) {
        if (delete_from_storage) {
            // I hope this will work
            for (auto item: grid[grid_index]) {
                map_objects.erase(item);
            }
        }
        grid[grid_index].clear();
    }

    // Delete specified object from provided tile
    void delete_object(int grid_index, int tile_index, bool delete_from_storage) {
        if (delete_from_storage) {
            map_objects.erase(grid[grid_index][tile_index]);
        }
        grid[grid_index].erase(grid[grid_index].begin() + tile_index);
    }

    // Place specific object from storage on specified space, without removing it
    // from any other place. Returns true for compatibility reasons - could be void
    bool place_object(size_t grid_index, int object_id) {
        grid[grid_index].push_back(object_id);

        return true;
    }

    // Move object from one tile to another.
    void move_object(int grid_index, int tile_index, int new_grid_index) {
        int object_id = grid[grid_index][tile_index];
        delete_object(grid_index, tile_index, false);
        place_object(new_grid_index, object_id);
    }

    // const T* get_tile_content(size_t grid_index);
    // Get pointer to object with specified id
    const T* get_object_by_id(int object_id) {
        return &map_objects.at(object_id);
    }

    // Get pointer to object from specified tile.
    // If return_placeholder is set to false and specified tile contains placeholder
    // tile - will return nullptr.
    // const T* get_object_from_grid(size_t grid_index) {
    //     int object_id = grid[grid_index];

    //     if (object_id == placeholder_id && !return_placeholder) {
    //         return nullptr;
    //     }

    //     return get_object_by_id(object_id);
    // }

    // Get first tile that contains object with specified id, or std::nullopt
    std::optional<Point> find_object_tile(int object_id) {
        for (auto index = 0u; index < grid_size; index++) {
            for (auto tile_i = 0u; tile_i < grid[index].size(); tile_i++) {
                if (grid[index][tile_i] == object_id) {
                    return index_to_tile(index);
                }
            }
        }

        return std::nullopt;
    }

    // Returns true if requrested things are within map's borders, false otherwise
    bool is_vec_on_map(Vector2 vec) {
        return (
            (0 <= vec.x) && (vec.x < map_real_size.x) && (0 <= vec.y) &&
            (vec.y < map_real_size.y));
    }

    bool is_index_on_map(size_t grid_index) {
        return ((0 <= grid_index) && (grid_index < grid_size));
    }

    bool is_tile_on_map(Point tile) {
        return (
            (0 <= tile.x) && (tile.x < map_size.x) && (0 <= tile.y) && (tile.y < map_size.y));
    }

    // Attempting to use out-of-bounds values on these will cause issues.
    // Thus don't forget to call is_<type>_on_map before using them!
    Point index_to_tile(size_t index) {
        int y = index / map_size.x;
        int x = index % map_size.x;
        return Point{x, y};
    }

    size_t tile_to_index(Point pos) {
        return static_cast<size_t>(pos.y * map_size.x + pos.x);
    }

    Vector2 index_to_vec(size_t index) {
        auto tile = index_to_tile(index);
        return Vector2{
            static_cast<float>(tile.x * tile_size.x),
            static_cast<float>(tile.y * tile_size.y)};
    }

    Point vec_to_tile(Vector2 vec) {
        return Point{
            static_cast<int>(vec.x) / tile_size.x,
            static_cast<int>(vec.y) / tile_size.y};
    }

    Vector2 tile_to_vec(Point tile) {
        return Vector2{
            static_cast<float>(tile.x * tile_size.x),
            static_cast<float>(tile.y * tile_size.y)};
    }

    size_t vec_to_index(Vector2 vec) {
        int x = vec.x / tile_size.x;
        int y = vec.y / tile_size.y;
        return static_cast<size_t>(y * map_size.x + x);
    }

    // Getters for various map-related stats
    Point get_tile_size() {
        return tile_size;
    }
    Point get_map_size() {
        return map_size;
    }
    size_t get_grid_size() {
        return grid_size;
    }
    // Returns entity ids of each object on map.
    std::vector<std::vector<int>> get_map_layout() {
        return grid;
    }

    void draw() {
        for (auto current_tile = 0u; current_tile < grid_size; current_tile++) {
            for (auto item : grid[current_tile]) {
                draw_callback(map_objects[item], index_to_vec(current_tile));
            }
        }
    }
};
