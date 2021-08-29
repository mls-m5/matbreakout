

#include "controls.h"
#include "matmath/vec2.h"
#include "sdlpp/events.hpp"
#include "sdlpp/render.hpp"
#include "sdlpp/window.hpp"
#include <array>
#include <iostream>
#include <vector>

int main(int argc, char **argv) {
    std::cout << "hello from " << argv[0] << std::endl;

    auto window = sdl::Window{"hello",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              300,
                              300,
                              SDL_WINDOW_SHOWN};

    auto renderer = sdl::Renderer{
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC};

    bool running = true;

    auto pos = Vec2{20, 20};
    auto controls = Controls{};
    auto axis = Vec2{};

    while (running) {
        while (auto event = sdl::pollEvents()) {
            controls.handleEvent(*event);

            if (event->type == SDL_QUIT) {
                running = false;
            }
        }

        axis.x = -controls.get(Controls::Left) + controls.get(Controls::Right);

        pos += axis;

        renderer.setDrawColor(0, 30, 0, 255);
        renderer.fillRect();

        renderer.setDrawColor(100, 100, 100, 255);
        renderer.drawLine(0, 0, 100, 100);

        auto rect =
            sdl::Rect{static_cast<int>(pos.x), static_cast<int>(pos.y), 20, 20};
        renderer.fillRect(rect);

        renderer.present();
    }
}
