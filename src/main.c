#include "config.h"
#include "map.h"
#include "player.h"
#include "ray.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <math.h>

typedef enum {
    WHITE = 0xFFFFFFFF,
    BLACK = 0x00000000,
    GREEN = 0xFF00FF00,
    DARK_GREEN = 0xFF199E19,
} Color;

#define CEILING WHITE
#define FLOOR BLACK

static int frame_buffer[WIDTH * HEIGHT];
static Hit hits[WIDTH];

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

void put_vertical_slice(int x, double distance, int color) {
    int height;
    if (distance == 0) {
        height = HEIGHT;
    } else {
        height = HEIGHT / distance;
    }
    int top = (HEIGHT - height) / 2;
    int bottom = HEIGHT - (HEIGHT - height) / 2;
    for (int i = 0; i < HEIGHT; i++) {
        int pixel_color;
        if (i < top) {
            pixel_color = CEILING;
        } else if (i > bottom) {
            pixel_color = FLOOR;
        } else {
            pixel_color = color;
        }
        put_pixel(x, i, pixel_color);
    }
}

void fill_buffer(Map *map, Player *player) {
    compute_hits(hits, map, player);

    clear(WHITE);
    for (int i = 0; i < WIDTH; i++) {
        Hit hit = hits[i];
        int color = hit.side == DIRECITON_X ? DARK_GREEN : GREEN;
        put_vertical_slice(i, hit.distance, color);
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

    Map map = {0};
    map_load(&map, "tests/maps/map3.txt");
    Player player = player_create(&map);

    SDL_Event event;
    while (running) {
        int start = SDL_GetPerformanceCounter();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }
        player_move_forward(&player, &map, 0.005);
        player_turn_left(&player, 0.1);

        fill_buffer(&map, &player);

        SDL_UpdateTexture(texture, NULL, frame_buffer, WIDTH * sizeof(int));
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        int end = SDL_GetPerformanceCounter();

        double elapsed =
            (double)(end - start) / (double)SDL_GetPerformanceFrequency();

        if (elapsed < TARGET_FRAME_LEN) {
            SDL_Delay((int)((TARGET_FRAME_LEN - elapsed) * 1000.0));
        }

        frame++;
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}