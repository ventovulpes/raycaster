#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

typedef enum { WHITE = 0xFFFFFFFF } Color;

#define WIDTH 800
#define HEIGHT 600

static int frame_buffer[WIDTH * HEIGHT];

void put_pixel(int x, int y, int color) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
        return;
    }
    frame_buffer[y * WIDTH + x] = color;
}

void clear(int color) {
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        frame_buffer[i] = color;
    }
}

int main(void) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow("Raycaster", WIDTH, HEIGHT, 0);
    if (!window) {
        SDL_Quit();
        return -1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_Texture *texture =
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_XRGB8888,
                          SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
    if (!texture) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    const double target_frame = 1.0 / 60.0;

    bool running = true;
    int frame = 1;

    SDL_Event event;
    while (running) {

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        clear(WHITE);

        SDL_UpdateTexture(texture, NULL, frame_buffer, WIDTH * sizeof(int));
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        frame++;
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}