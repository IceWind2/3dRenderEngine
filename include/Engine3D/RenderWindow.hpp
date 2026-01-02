#pragma once

#include <SDL3/SDL.h>
#include <Engine3D/VectorMatrix.hpp>

class RenderWindow {
private:
    SDL_Window* _window = nullptr;
    SDL_Renderer* _renderer = nullptr;
public:
    RenderWindow(const char *title, int width, int height);
    ~RenderWindow();
    
    void SetTitle(const char* title) {
        SDL_SetWindowTitle(_window, title);
    }
    void DrawTriangle(const triangle& tri, const float lum) const;
    void ClearScreen() const;
    void Present() const;
};