#include <SDL2/SDL.h>
#include <iostream>

#include "RenderWindow.hpp"

RenderWindow::RenderWindow(const char *title, int width, int height)
    : window(NULL), renderer(NULL) {

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);

    if (window == NULL) {
        std::string errorMessage = "Window failed to init. " + (std::string)(SDL_GetError());
        throw std::runtime_error(errorMessage);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

void RenderWindow::RenderFrame(const SDL_Point *points) {
    SDL_RenderDrawPoints(renderer, points, 1);
    SDL_RenderPresent(renderer);
}

RenderWindow::~RenderWindow() {
    SDL_DestroyWindow(window);

    SDL_DestroyRenderer(renderer);
}