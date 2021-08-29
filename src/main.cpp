

#include "sdlpp/events.hpp"
#include "sdlpp/render.hpp"
#include "sdlpp/window.hpp"
#include "worldimpl.h"

int main(int argc, char **argv) {
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
