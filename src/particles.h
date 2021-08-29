#pragma once

#include "constants.h"
#include "sdlpp/render.hpp"
#include <algorithm>
#include <random>
#include <vector>

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

            renderer.drawColor(p.r, p.g, p.b, alpha);
            renderer.drawPointF(p);
        }
    }
};
