#pragma once

#include <SDL2/SDL.h>
#include <vector>

class Frame {
private:
    const std::vector<std::tuple<int, int, int>> _colours {
        {0, 0, 0},                // black == 0
        {255, 255, 255}           // white == 1
    };
    
    std::vector<std::vector<SDL_Point>> _pixels;

public:
    Frame(const std::vector<std::vector<int>>& points);

    void RenderTo(SDL_Renderer* renderer) const;
};