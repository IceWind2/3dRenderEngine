#include <SDL3/SDL.h>
#include <cstdio>
#include <string>
#include <stdexcept>

#include "Engine3D/RenderWindow.hpp"
#include "SDL3/SDL_render.h"

RenderWindow::RenderWindow(const char *title, int width, int height){
    _window = SDL_CreateWindow(title, width, height, SDL_WINDOW_VULKAN);

    if (_window == NULL) {
        std::string errorMessage = "Window failed to init. " + (std::string)(SDL_GetError());
        throw std::runtime_error(errorMessage);
    }

    _renderer = SDL_CreateRenderer(_window, NULL);
    SDL_RenderClear(_renderer);
    SDL_RenderPresent(_renderer);
}

RenderWindow::~RenderWindow() {
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
}

void RenderWindow::DrawTriangle(const triangle& tri, const float lum) const{
    SDL_FPoint points[4];
    points[0].x = tri.p[0].x;
    points[0].y = tri.p[0].y;
    points[1].x = tri.p[1].x;
    points[1].y = tri.p[1].y;
    points[2].x = tri.p[2].x;
    points[2].y = tri.p[2].y;
    points[3] = points[0];
    SDL_Vertex vertices[3];
    vertices[0].position = points[0];
    vertices[1].position = points[1];
    vertices[2].position = points[2];
    vertices[0].color = {lum, lum, lum, 255};
    vertices[1].color = {lum, lum, lum, 255};
    vertices[2].color = {lum, lum, lum, 255};

    SDL_RenderGeometry(_renderer, NULL, vertices, 3, NULL, 0);

    // Draw wireframe
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
    SDL_RenderLines(_renderer, points, 4);
}

void RenderWindow::Present() const{
    SDL_RenderPresent(_renderer);
}

void RenderWindow::ClearScreen() const{
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
    SDL_RenderClear(_renderer);
}
