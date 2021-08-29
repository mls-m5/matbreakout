#pragma once

#include "matmath/vec2.h"
#include "sdlpp/render.hpp"

struct Bar {
    Vec2f size{60, 4};
    Vec2f pos{20, 30};

    void draw(sdl::Renderer &renderer) {
        renderer.setDrawColor(100, 100, 100, 255);
        renderer.fillRect({static_cast<int>(pos.x - size.x / 2.),
                           static_cast<int>(pos.y - size.y / 2.),
                           static_cast<int>(size.x),
                           static_cast<int>(size.y)});
    }

    bool isInside(Vec2f p) {
        if (p.x < pos.x - size.x / 2.) {
            return false;
        }

        if (p.x > pos.x + size.x / 2.) {
            return false;
        }

        if (p.y < pos.y) {
            return false;
        }

        return true;
    }
};
