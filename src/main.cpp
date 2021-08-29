

#include "SDL2/SDL_keycode.h"
#include "matmath/vec2.h"
#include "sdlpp/events.hpp"
#include "sdlpp/render.hpp"
#include "sdlpp/window.hpp"
#include <array>
#include <iostream>
#include <vector>

struct Controls {
    Vec2 movement;

    enum CN {
        None,
        Left,
        Right,
        Count,
    };

    std::vector<std::pair<CN, int>> controlMap = {
        {Left, SDL_SCANCODE_LEFT},
        {Right, SDL_SCANCODE_RIGHT},
    };

    std::array<int, Count> state;

    auto get(CN c) {
        return state.at(c);
    }

    auto scancodeToControl(const SDL_Event &event) {
        for (auto c : controlMap) {
            if (c.second == event.key.keysym.scancode) {
                return c.first;
            }
        }

        return None;
    }

    void handleEvent(SDL_Event &event) {

        switch (event.type) {
        case SDL_KEYDOWN: {
            auto control = scancodeToControl(event);
            state.at(control) = 1;
            break;
        }
        case SDL_KEYUP: {
            auto control = scancodeToControl(event);
            state.at(control) = 0;
            break;
        }
        default:
            return;
        }
    }
};

int main(int argc, char **argv) {
    std::cout << "hello from " << argv[0] << std::endl;

    auto window = sdl::Window{"hello",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              300,
                              300,
                              SDL_WINDOW_SHOWN};

    auto renderer = sdl::Renderer{window, -1, SDL_RENDERER_ACCELERATED};

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

        renderer.setDrawColor(100, 100, 100, 255);
        renderer.drawLine(0, 0, 100, 100);

        auto rect =
            sdl::Rect{static_cast<int>(pos.x), static_cast<int>(pos.y), 20, 20};
        renderer.fillRect(rect);

        renderer.present();
    }
}
