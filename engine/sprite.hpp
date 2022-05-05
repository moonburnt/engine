#pragma once

#include "utility.hpp"
#include <raylib.h>
#include <vector>

class Sprite {
private:
    const Texture2D* spritesheet;
    Rectangle rect;

public:
    Sprite(const Texture2D* spritesheet, Rectangle rect);
    // TODO: consider using DrawTexturePro under the hood, to allow for angle
    // and colored masks.
    void draw(Vector2 pos);
};

// Turn rectangle on specified coordinates into Sprite. If rectangle would be out
// of bounds - will throw an exception.
Sprite make_sprite(const Texture2D* spritesheet, Rectangle sprite_rect);

// Get set of of same-sized rectangles from texture. If spritesheet doesn't
// cut into these perfectly - will throw an exception.
std::vector<Rectangle> make_rects(const Texture2D* spritesheet, Vector2 sprite_size);
// Same as above, but will also convert rects to sprites.
std::vector<Sprite> make_sprites(const Texture2D* spritesheet, Vector2 sprite_size);

// TODO: maybe make it possible for animation to be both based on spritesheet
// and separate textures? Thus this whole thing will become pointless
template <typename T> class AnimationBase {
protected:
    Timer timer;
    std::vector<T> frames;
    bool loop;
    size_t current_frame = 0;

public:
    Vector2 pos;

    // Maybe I should make it not vector, but something less memory-consuming? TODO
    AnimationBase(std::vector<T> _frames, float speed, bool _loop, Vector2 _pos)
        : timer(speed)
        , frames(_frames)
        , loop(_loop)
        , pos(_pos) {
        timer.start();
    }
    virtual ~AnimationBase() = default;

    // Manually current frame to provided value. Doing so will stop timer.
    void set_current_frame(size_t frame) {
        current_frame = frame;
        timer.stop();
    }

    void update(float dt) {
        if (timer.tick(dt)) {
            if (current_frame + 1 < frames.size()) {
                current_frame++;
                timer.start();
            }
            else if (loop) {
                current_frame = 0;
                timer.start();
            }
            else {
                timer.stop();
            }
        };
    }

    virtual void draw() = 0;
};

class Animation : public AnimationBase<const Texture2D*> {
public:
    void draw() override;
};

class SpritesheetAnimation : public AnimationBase<Rectangle> {
protected:
    const Texture2D* spritesheet;

public:
    SpritesheetAnimation(
        const Texture2D* spritesheet,
        std::vector<Rectangle> frames,
        float speed,
        bool loop,
        Vector2 pos);

    void draw() override;
};
