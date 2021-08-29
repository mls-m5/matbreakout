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

        balls.emplace_back();

        for (int y = 0; y < 10; ++y) {
            for (int x = 0; x < 10; ++x) {
                bricks.push_back({Vec2f{x * Brick::brickSize.x,
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

    void addParticle(Vec2f p, Vec2f v) override {
        particles.particles.push_back({p.x, p.y, v.x, v.y});
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

    Collision collide(Vec2f p, Vec2f size) override {
        for (auto &b : bricks) {
            if (b.isInside(p, size)) {
                return {
                    .normal = {},
                    .isCollision = true,
                    .brick = &b,
                };
            }
        }

        return {};
    }
};