#pragma once

#include <SDL3/SDL.h>
#include <vector>

class Frame {
private:
    const std::vector<std::tuple<int, int, int>> _colours {
        {0, 0, 0},                // black == 0
        {255, 255, 255}           // white == 1
    };
    
    std::vector<std::vector<SDL_FPoint>> _pixels;

public:
    Frame(const std::vector<std::vector<float>>& points);

    void RenderTo(SDL_Renderer* renderer) const;
};