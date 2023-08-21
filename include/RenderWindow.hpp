#pragma once

#include <SDL2/SDL.h>

class RenderWindow {
private:
    SDL_Window *window;
    SDL_Renderer *renderer;

public:
    RenderWindow(const char *title, int width, int height);

    void RenderFrame(const SDL_Point *points);

    ~RenderWindow();
};