#pragma once

#include <SDL2/SDL.h>
#include "Frame.hpp"

class RenderWindow {
private:
    SDL_Window* _window;
    SDL_Renderer* _renderer;

public:
    RenderWindow(const char *title, int width, int height);

    void RenderFrame(const Frame frame);

    ~RenderWindow();
};