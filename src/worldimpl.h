#pragma once

#include "ball.h"
#include "bar.h"
#include "brick.h"
#include "particles.h"
#include "world.h"
#include <vector>

struct WorldImpl : public World {
    Bar bar;
    std::vector<Ball> balls;
    Particles particles;
    std::vector<Brick> bricks;

    WorldImpl(Vec2 worldSize) {
        width = worldSize.x;
        height = worldSize.y;

        bar.pos.y = height - bar.size.y * 2;

        balls.emplace_back().pos.y = height - 20 - 1;

        int maxX = width / Brick::brickSize.x;

        for (int y = 0; y < 10; ++y) {
            for (int x = 0; x < maxX; ++x) {
                bricks.push_back({Vec2f{(x + .5f) * Brick::brickSize.x,
                                        y * Brick::brickSize.y + 40}});
            }
        }
    }

    void update(World &world) {
        auto axis = Vec2f{};
        auto &controls = world.controls;

        for (auto &b : balls) {
            b.update(*this);
        }

        particles.update();

        axis.x = -controls.get(Controls::Left) + controls.get(Controls::Right);

        bar.pos += axis * 10;

        bar.pos.x = std::min(std::max(0.f, bar.pos.x), width);

        auto it = std::remove_if(
            bricks.begin(), bricks.end(), [](auto &b) { return b.dead; });

        bricks.erase(it, bricks.end());
    }

    void draw(sdl::Renderer &renderer) {
        particles.draw(renderer);
        for (auto &ball : balls) {
            ball.draw(renderer);
        }

        bar.draw(renderer);

        for (auto &brick : bricks) {
            brick.draw(renderer);
        }
    }

    Particle &addParticle(Vec2f p, Vec2f v) override {
        particles.particles.push_back({p.x, p.y, v.x, v.y});
        return particles.particles.back();
    }

    bool isInsideBar(Vec2f p) override {
        return bar.isInside(p);
    }

    void triggerEvent(Event event) override {
        if (event.num == World::HitBar) {
            constexpr int num = 40;
            for (int i = -num; i < num; ++i) {
                addParticle(
                    {bar.pos.x + static_cast<float>(i) / num * bar.size.x / 2.f,
                     bar.pos.y},
                    {0, -.1});
            }
        }
    }

    Collision collide(Vec2f p, Vec2f halfSize) override {
        for (auto &b : bricks) {
            if (b.isInside(p, halfSize)) {
                auto normal = toNormal(b.getCollisionPart(p));
                return {
                    .isCollision = true,
                    .brick = &b,
                    .normal = normal,
                };
            }
        }

        return {};
    }

    void explosion(Vec2f pos, ExplosionInfo info) override {
        for (int i = 0; i < info.numParticles; ++i) {
            auto &p = addParticle(
                pos,
                {(static_cast<float>(rand()) / static_cast<float>(RAND_MAX) -
                  .5f) *
                     info.power,
                 (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) -
                  .5f) *
                     info.power});

            p.spread = info.spread;
            p.vx = info.vel.x;
            p.vy = info.vel.y;
        }
    }

    float barAmount(Vec2f p) override {
        return bar.amount(p);
    }
};
