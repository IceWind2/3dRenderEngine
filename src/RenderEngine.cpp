#include <SDL2/SDL.h>
#include <chrono>
#include <thread>

#include "RenderEngine.hpp"
#include "RenderWindow.hpp"
#include "Frame.hpp"

void RenderEngine::Start() {
    _engineRunning = true;

    SDL_Event event;

    std::vector<std::vector<int>> points {{0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};
    
    Frame frame(points);

    while (_engineRunning) {
        _rWindow.RenderFrame(frame);

        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                _engineRunning = false;
            }
        }
    }
}