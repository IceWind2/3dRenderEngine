#include <vector>
#include <SDL2/SDL.h>

#include "Frame.hpp"

Frame::Frame(const std::vector<std::vector<int>>& points)
    : _pixels(_colours.size()) {

    for (size_t i = 0; i < points.size(); ++i) {
        for (size_t j = 0; j < points[0].size(); ++j) {
            SDL_Point pixel {(int)i, (int)j};

            _pixels[points[i][j]].push_back(pixel);
        }
    }
}

void Frame::RenderTo(SDL_Renderer* renderer) const {
    for (size_t i = 0; i < _colours.size(); ++i) {
        SDL_SetRenderDrawColor(renderer,
                               std::get<0>(_colours[i]),
                               std::get<1>(_colours[i]),
                               std::get<2>(_colours[i]),
                               255);

        SDL_RenderDrawPoints(renderer, &_pixels[i][0], _pixels[i].size());
    }

    SDL_RenderPresent(renderer);
}