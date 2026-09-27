#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

int main(void) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow("Raycaster", 800, 600, 0);
    if (!window) {
        SDL_Quit();
        return -1;
    }

    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}