#include <SDL2/SDL.h>
#include <iostream>

#include "RenderWindow.hpp"
#include "Frame.hpp"

RenderWindow::RenderWindow(const char *title, int width, int height)
    : _window(NULL), _renderer(NULL) {

    _window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);

    if (_window == NULL) {
        std::string errorMessage = "Window failed to init. " + (std::string)(SDL_GetError());
        throw std::runtime_error(errorMessage);
    }

    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
}

void RenderWindow::RenderFrame(const Frame frame) {
    frame.RenderTo(_renderer);
}

RenderWindow::~RenderWindow() {
    SDL_DestroyWindow(_window);

    SDL_DestroyRenderer(_renderer);
}