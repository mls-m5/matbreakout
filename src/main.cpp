

#include "controls.h"
#include "matmath/vec2.h"
#include "sdlpp/events.hpp"
#include "sdlpp/render.hpp"
#include "sdlpp/window.hpp"
#include <array>
#include <iostream>
#include <vector>

double t = 1. / 60.;

struct Particle {
    float x, y;
    float duration = 1;
    char r = '\255', g = '\255', b = '\255', dead = 0;

    void update() {
        duration -= t;
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
            renderer.drawPointF(p);
        }
    }
};

struct World {
    double width;
    double height;

    Controls controls;
    Particles particles;

    virtual ~World() = default;
};

struct Bar {
    Vec2 size{60, 20};
    Vec2 pos{20, 30};

    void draw(sdl::Renderer &renderer) {
        renderer.setDrawColor(100, 100, 100, 255);
        renderer.fillRect({static_cast<int>(pos.x - size.x / 2.),
                           static_cast<int>(pos.y - size.y / 2.),
                           static_cast<int>(size.x),
                           static_cast<int>(size.y)});
    }
};

struct Ball {
    Vec2 size{20, 20};
    Vec2 pos{20, 30};

    Vec2 dir{1, 1};

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
    }
};

struct WorldImpl : public World {
    Bar bar;
    std::vector<Ball> balls;

    WorldImpl(Vec2 worldSize) {
        width = worldSize.x;
        height = worldSize.y;

        bar.pos.y = height - bar.size.y * 2;

        balls.emplace_back();
    }

    void update(World &world) {
        auto axis = Vec2{};
        auto &controls = world.controls;

        for (auto &b : balls) {
            b.update(*this);
        }

        particles.update();

        axis.x = -controls.get(Controls::Left) + controls.get(Controls::Right);

        bar.pos += axis * 10;

        bar.pos.x = std::min(std::max(0., bar.pos.x), width);
    }

    void draw(sdl::Renderer &renderer) {
        for (auto &ball : balls) {
            ball.draw(renderer);
        }

        bar.draw(renderer);
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

    auto world = WorldImpl{{width, height}};

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
