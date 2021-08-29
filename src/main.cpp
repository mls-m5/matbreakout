

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
    float duration = 5;
    float maxDuration = 5;
    char r = '\255', g = '\255', b = '\255', dead = 0;

    void update() {
        duration -= t;
        x += static_cast<double>(rand()) / RAND_MAX - .5;
        y += static_cast<double>(rand()) / RAND_MAX - .5;
        dead = duration < 0;
    }
};

struct Particles {
    std::vector<Particle> particles;

    void update() {
        for (auto &p : particles) {
            p.update();
        }
    }

    void draw(sdl::Renderer &renderer) {
        // Prabably really bad performance here
        for (auto &p : particles) {
            auto alpha = static_cast<int>(p.duration / p.maxDuration * 255.);

            renderer.setDrawColor(p.r, p.g, p.b, alpha);
            //            renderer.setDrawColor(alpha, alpha, alpha, alpha);
            renderer.drawPointF(p);
        }

        auto it = std::remove_if(
            particles.begin(), particles.end(), [](auto &p) { return p.dead; });

        particles.erase(it, particles.end());
    }
};

struct World {
    float width;
    float height;

    Controls controls;
    Particles particles;

    virtual ~World() = default;

    virtual void addParticle(Vec2f p) = 0;
    virtual bool isInsideBar(Vec2f p) = 0;
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

            // Some event
        }

        world.addParticle(pos);
    }
};

struct WorldImpl : public World {
    Bar bar;
    std::vector<Ball> balls;
    Particles particles;

    WorldImpl(Vec2 worldSize) {
        width = worldSize.x;
        height = worldSize.y;

        bar.pos.y = height - bar.size.y * 2;

        balls.emplace_back();
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
    }

    void draw(sdl::Renderer &renderer) {
        particles.draw(renderer);
        for (auto &ball : balls) {
            ball.draw(renderer);
        }

        bar.draw(renderer);
    }

    void addParticle(Vec2f p) override {
        particles.particles.push_back({p.x, p.y});
    }

    bool isInsideBar(Vec2f p) override {
        return bar.isInside(p);
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
