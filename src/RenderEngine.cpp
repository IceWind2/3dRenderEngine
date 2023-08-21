#include <SDL2/SDL.h>
#include <chrono>
#include <thread>

#include "RenderEngine.hpp"
#include "RenderWindow.hpp"

void RenderEngine::Start() {
    _engineRunning = true;

    SDL_Event event;

    SDL_Point *points = new SDL_Point[1];
    points[0].x = 50;
    points[0].y = 50;
    int cst = 100;

    while (_engineRunning) {
        _rWindow.RenderFrame(points);

        points[0].x += cst;
        points[1].y += cst;

        cst *= -1;

        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                _engineRunning = false;
            }
        }
    }

    delete[] points;
}