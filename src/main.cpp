#include <SDL2/SDL.h>
#include <iostream>

#include "render/RenderEngine.hpp"

int main(int argc, char *args[]) {
    std::cout << "Init SDL" << std::endl;

    if (SDL_Init(SDL_INIT_VIDEO) > 0) {
        std::cout << "SDL_Init error " << SDL_GetError() << std::endl;

        return 1;
    }

    try {
        RenderEngine rEngine;

        rEngine.Start();
    }
    catch (std::exception &ex) {
        std::cout << ex.what() << std::endl;
    }

    std::cout << "Close SDL" << std::endl;

    SDL_Quit();

    return 0;
}