#pragma once

#include "matmath/vec2.h"
#include "sdlpp/render.hpp"

struct Brick {
    static constexpr Vec2f brickSize = {20, 10};

    Vec2f pos;
    char r = rand() % 200, g = rand() % 200, b = rand() % 200;
    Vec2f size = brickSize;
    bool dead = false;

    void draw(sdl::Renderer &renderer) {
        renderer.setDrawColor(r, g, b, 255);
        renderer.fillRect({static_cast<int>(pos.x - size.x / 2.),
                           static_cast<int>(pos.y - size.y / 2.),
                           static_cast<int>(size.x),
                           static_cast<int>(size.y)});
    }

    bool isInside(Vec2f p, Vec2f s) {
        if (p.x + s.x > pos.x + size.x) {
            return false;
        }
        if (p.x - s.x < pos.x - size.x) {
            return false;
        }
        if (p.y + s.y > pos.y + size.y) {
            return false;
        }
        if (p.y - s.y < pos.y - size.y) {
            return false;
        }
        return true;
    }
};

struct Collision {
    Vec2f normal;
    bool isCollision = false;
    Brick *brick = nullptr;

    operator bool() {
        return isCollision;
    }
};
