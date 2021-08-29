#pragma once

#include "matmath/vec2.h"
#include "sdlpp/render.hpp"

enum class BrickPart {
    Top,
    Bottom,
    Left,
    Right,
};

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
        if (p.x + s.x > pos.x + halfWidth()) {
            return false;
        }
        if (p.x - s.x < pos.x - halfWidth()) {
            return false;
        }
        if (p.y + s.y > pos.y + halfHeight()) {
            return false;
        }
        if (p.y - s.y < pos.y - halfHeight()) {
            return false;
        }
        return true;
    }

    float halfWidth() {
        return size.x / 2.f;
    }

    float halfHeight() {
        return size.y / 2.f;
    }

    //! Only use if collision is already verified
    auto getCollisionPart(Vec2f p) {
        auto part = BrickPart::Top;
        auto nearest = std::abs(p.y - pos.y + halfHeight());

        if (auto nnearest = std::abs(p.y - pos.y - halfHeight());
            nnearest < nearest) {
            nnearest = nearest;

            part = BrickPart::Bottom;
        }

        if (auto nnearest = std::abs(p.x - pos.x - halfWidth());
            nnearest < nearest) {
            nnearest = nearest;
            part = BrickPart::Right;
        }

        if (auto nnearest = std::abs(p.x - pos.x + halfWidth());
            nnearest < nearest) {
            nnearest = nearest;
            part = BrickPart::Left;
        }

        return part;
    }
};

struct Collision {
    bool isCollision = false;
    Brick *brick = nullptr;
    Vec2f normal;

    operator bool() {
        return isCollision;
    }
};

BrickPart findPart(Vec2f normal) {
    if (std::abs(normal.x) > std::abs(normal.y)) {
        if (normal.x > 0) {
            return BrickPart::Right;
        }
        else {
            return BrickPart::Left;
        }
    }
    else {
        if (normal.y > 0) {
            return BrickPart::Bottom;
        }
        else {
            return BrickPart::Top;
        }
    }
}

Vec2f toNormal(BrickPart part) {
    switch (part) {
    case BrickPart::Bottom:
        return {0, 1};
    case BrickPart::Top:
        return {0, -1};
    case BrickPart::Left:
        return {-1, 0};
    case BrickPart::Right:
        return {1, 0};
    }
}
