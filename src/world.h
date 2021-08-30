#pragma once

#include "controls.h"
#include "matmath/vec2.h"

struct World {
    enum EventNum {
        HitBar,
        BallOutside,
    };

    struct Event {
        EventNum num;
        Vec2f position;
    };

    float width;
    float height;

    Controls controls;

    virtual ~World() = default;

    virtual struct Particle &addParticle(Vec2f p, Vec2f v = {}) = 0;
    virtual bool isInsideBar(Vec2f p) = 0;
    virtual void triggerEvent(Event event) = 0;

    // Test ball collision with bricks
    virtual struct Collision collide(Vec2f p, Vec2f halfSize) = 0;

    struct ExplosionInfo {
        Vec2f vel;
        float spread = 1;
        float power = 10;
        int numParticles = 100;
    };

    virtual void explosion(Vec2f pos, ExplosionInfo) = 0;

    virtual float barAmount(Vec2f p) = 0;
};
