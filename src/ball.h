#pragma once

#include "brick.h"
#include "matmath/vec2.h"
#include "world.h"

struct Ball {
    Vec2f size{6, 6};
    Vec2f pos{20, 30};

    Vec2f dir{1, -1};

    void draw(sdl::Renderer &renderer) {
        renderer.drawColor(200, 200, 200, 255);
        renderer.fillRect({static_cast<int>(pos.x - size.x / 2.),
                           static_cast<int>(pos.y - size.y / 2.),
                           static_cast<int>(size.x),
                           static_cast<int>(size.y)});
    }

    void update(World &world) {
        pos += dir;

        dir = dir / dir.abs();
        dir *= 2;

        if (pos.x > world.width) {
            dir.x = -std::abs(dir.x);
        }
        else if (pos.x < 0) {
            dir.x = std::abs(dir.x);
        }

        if (pos.y < 0) {
            dir.y = std::abs(dir.y);
        }
        else if (pos.y > world.height) {
            dir.y = -std::abs(dir.y);
        }

        if (dir.y > 0 && world.isInsideBar(pos)) {
            dir.y = -std::abs(dir.y);

            world.triggerEvent({World::HitBar, pos});

            dir.x = world.barAmount(pos) * std::abs(dir.y);
        }

        world.addParticle(pos);

        if (auto c = world.collide(pos, size / 2.)) {
            c.brick->dead = true;

            if (std::abs(c.normal.y) > std::abs(c.normal.x)) {
                dir.y *= -1;
            }
            else {
                dir.x *= -1;
            }

            auto ex = World::ExplosionInfo{};

            ex.power = 1;
            ex.spread = 4;
            ex.vel = c.normal * .5;

            world.explosion(pos, ex);
        }
    }
};
