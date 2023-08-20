#include <SDL2/SDL.h>
#include <iostream>

#include "RenderWindow.hpp"

int main(int argc, char *args[])
{
    if (SDL_Init(SDL_INIT_VIDEO) > 0)
    {
        std::cout << "SDL_Init error " << SDL_GetError() << std::endl;
    }

    RenderWindow window("Test window", 1280, 720);

    bool engineRunning = true;

    SDL_Event event;

    while (engineRunning)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                engineRunning = false;
            }
        }
    }

    SDL_Quit();

    return 0;
}