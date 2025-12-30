#pragma once

#include <SDL3/SDL.h>
#include <Engine3D/MathTypes.hpp>

class RenderWindow {
private:
    SDL_Window* _window = nullptr;
    SDL_Renderer* _renderer = nullptr;
public:
    RenderWindow(const char *title, int width, int height);
    ~RenderWindow();
    
    void DrawTriangle(const triangle* tri) const;
};