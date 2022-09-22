#include "sprite.hpp"
#include "utility.hpp"

#include "raylib.h"

Sprite::Sprite(const Texture2D* _spritesheet, Rectangle _rect)
    : spritesheet(_spritesheet)
    , rect(_rect) {
}

void Sprite::draw(Vector2 pos) {
    DrawTextureRec(*spritesheet, rect, pos, WHITE);
}

Sprite make_sprite(const Texture2D* spritesheet, Rectangle sprite_rect) {
    // I think this will work? Idk how else to ensure if sprite rect is within
    // texture's borders. Maybe I should throw exception instead... TODO.
    ASSERT(is_rect_inside_rect(
        Rectangle{
            0.0f,
            0.0f,
            static_cast<float>(spritesheet->width),
            static_cast<float>(spritesheet->height)},
        sprite_rect));

    return Sprite(spritesheet, sprite_rect);
}

std::vector<Rectangle> make_rects(const Texture2D* spritesheet, Vector2 sprite_size) {
    // Ensuring that spritesheet can be cut to sprites of requested size without
    // leftovers.
    // TODO: maybe add some arg to specify some part of spritesheet to ignore.

    ASSERT(spritesheet->width % static_cast<int>(sprite_size.x) == 0);
    ASSERT(spritesheet->height % static_cast<int>(sprite_size.y) == 0);

    int horizontal_amount = spritesheet->width / sprite_size.x;
    int vertical_amount = spritesheet->height / sprite_size.y;

    std::vector<Rectangle> rects;

    // I think this will do? May need to flip these around.
    for (auto current_y = 0; current_y < vertical_amount; current_y++) {
        for (auto current_x = 0; current_x < horizontal_amount; current_x++) {
            rects.push_back(
                {static_cast<float>(sprite_size.x * current_x),
                 static_cast<float>(sprite_size.y * current_y),
                 sprite_size.x,
                 sprite_size.y});
        }
    }

    return rects;
}

std::vector<Sprite> make_sprites(const Texture2D* spritesheet, Vector2 sprite_size) {
    std::vector<Sprite> sprites;

    for (auto rect : make_rects(spritesheet, sprite_size)) {
        sprites.push_back(Sprite(spritesheet, rect));
    }

    return sprites;
}

// Animation stuff
void Animation::draw() {
    DrawTextureV(*frames[current_frame], get_world_pos(), WHITE);
}

// Spritesheet animation stuff
SpritesheetAnimation::SpritesheetAnimation(
    const Texture2D* _spritesheet,
    std::vector<Rectangle> _frames,
    float speed,
    bool _loop,
    Vector2 _pos)
    : AnimationBase<Rectangle>(_frames, speed, _loop, _pos)
    , spritesheet(_spritesheet) {
}

void SpritesheetAnimation::draw() {
    DrawTextureRec(*spritesheet, frames[current_frame], get_world_pos(), WHITE);
}
