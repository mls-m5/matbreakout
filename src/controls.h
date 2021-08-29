#pragma once

#include "SDL2/SDL_events.h"
#include "SDL2/SDL_keycode.h"
#include <array>
#include <matmath/vec2.h>
#include <vector>

struct Controls {
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
