

#include "SDL2/SDL_opengl.h"
#include "controls.h"
#include "matmath/vec2.h"
#include "sdlpp/events.hpp"
#include "sdlpp/render.hpp"
#include "sdlpp/window.hpp"
#include <algorithm>
#include <array>
#include <iostream>
#include <vector>

double t = 1. / 60.;

struct Particle {
    float x, y;
    float vx = 0, vy = 0;
    float duration = 2;
    float maxDuration = 2;
    float spread = 2;
    char r = '\255', g = '\255', b = '\255', dead = 0;

    void update() {
        duration -= t;
        x += vx;
        y += vy;
        x += (static_cast<double>(rand()) / RAND_MAX - .5) * spread;
        y += (static_cast<double>(rand()) / RAND_MAX - .5) * spread;
        dead = duration < 0;
    }
};

struct Particles {
    std::vector<Particle> particles;

    void update() {
        for (auto &p : particles) {
            p.update();
        }

        auto it = std::remove_if(
            particles.begin(), particles.end(), [](auto &p) { return p.dead; });

        particles.erase(it, particles.end());
    }

    void draw(sdl::Renderer &renderer) {
        // Prabably really bad performance here
        for (auto &p : particles) {
            auto alpha = static_cast<int>(p.duration / p.maxDuration * 255.);

            renderer.setDrawColor(p.r, p.g, p.b, alpha);
            renderer.drawPointF(p);
        }
    }
};

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

struct Ball {
    Vec2f size{2, 2};
    Vec2f pos{20, 30};

    Vec2f dir{1, 1};

    void draw(sdl::Renderer &renderer) {
        renderer.setDrawColor(200, 200, 200, 255);
        renderer.fillRect({static_cast<int>(pos.x - size.x / 2.),
                           static_cast<int>(pos.y - size.y / 2.),
                           static_cast<int>(size.x),
                           static_cast<int>(size.y)});
    }

    void update(World &world) {
        pos += dir;

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
        }

        world.addParticle(pos);

        if (auto c = world.collide(pos, size)) {
            c.brick->dead = true;
        }
    }
};

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

int main(int argc, char **argv) {
    std::cout << "hello from " << argv[0] << std::endl;

    const auto width = 300;
    const auto height = 300;

    auto window = sdl::Window{"hello",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              width,
                              height,
                              SDL_WINDOW_SHOWN};

    auto renderer = sdl::Renderer{
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC};

    bool running = true;

    glEnable(GL_BLEND);

    auto world = WorldImpl{{width, height}};

    renderer.drawBlendMode(SDL_BLENDMODE_BLEND);

    while (running) {
        while (auto event = sdl::pollEvents()) {
            world.controls.handleEvent(*event);

            if (event->type == SDL_QUIT) {
                running = false;
            }
        }

        world.update(world);

        renderer.setDrawColor(0, 30, 0, 255);
        renderer.fillRect();

        world.draw(renderer);

        renderer.present();
    }
}
