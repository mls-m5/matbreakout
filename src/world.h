#pragma once

#include "controls.h"
#include "matmath/vec2.h"

struct World {
    enum EventNum { HitBar };

    struct Event {
        EventNum num;
        Vec2f position;
    };

    float width;
    float height;

    Controls controls;

    virtual ~World() = default;

    virtual void addParticle(Vec2f p, Vec2f v = {}) = 0;
    virtual bool isInsideBar(Vec2f p) = 0;
    virtual void triggerEvent(Event event) = 0;

    // Test ball collision with bricks
    virtual struct Collision collide(Vec2f p, Vec2f size) = 0;

    void destroyBrick(const struct Brick &);
};
