#include <SDL3/SDL.h>
#include <string>
#include <stdexcept>

#include "render/RenderWindow.hpp"
#include "render/Frame.hpp"

RenderWindow::RenderWindow(const char *title, int width, int height)
    : _window(NULL), _renderer(NULL) {

    _window = SDL_CreateWindow(title, width, height, SDL_WINDOW_VULKAN);

    if (_window == NULL) {
        std::string errorMessage = "Window failed to init. " + (std::string)(SDL_GetError());
        throw std::runtime_error(errorMessage);
    }

    _renderer = SDL_CreateRenderer(_window, NULL);
    SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
    SDL_RenderClear(_renderer);
    SDL_RenderPresent(_renderer);
}

void RenderWindow::RenderFrame(const Frame frame) {
    frame.RenderTo(_renderer);
}

RenderWindow::~RenderWindow() {
    SDL_DestroyWindow(_window);

    SDL_DestroyRenderer(_renderer);
}