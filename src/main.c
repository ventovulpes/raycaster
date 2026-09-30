#include "config.h"
#include "map.h"
#include "player.h"
#include "ray.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <math.h>
#include <stdio.h>

typedef enum {
    WHITE = 0xFFFFFFFF,
    BLACK = 0x00000000,
    GREEN = 0xFF00FF00,
    DARK_GREEN = 0xFF199E19,
} Color;

typedef enum { KEY_UP = 001, KEY_LEFT = 010, KEY_RIGHT = 100 } Key;

#define CEILING_COLOR WHITE
#define FLOOR_COLOR BLACK

static int frame_buffer[WIDTH * HEIGHT];
static Hit hits[WIDTH];
// bitmap which stores which keys are being pressed
static int keys_down = 0;

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
            pixel_color = CEILING_COLOR;
        } else if (i > bottom) {
            pixel_color = FLOOR_COLOR;
        } else {
            pixel_color = color;
        }
        put_pixel(x, i, pixel_color);
    }
}

void handle_input(bool *running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            running = false;
        } else if (event.type == SDL_EVENT_KEY_DOWN) {
            switch (event.key.key) {
            case SDLK_UP:
                keys_down |= KEY_UP;
                break;
            case SDLK_LEFT:
                keys_down |= KEY_LEFT;
                break;
            case SDLK_RIGHT:
                keys_down |= KEY_RIGHT;
                break;
            }
        } else if (event.type == SDL_EVENT_KEY_UP) {
            switch (event.key.key) {
            case SDLK_UP:
                keys_down ^= KEY_UP;
                break;
            case SDLK_LEFT:
                keys_down ^= KEY_LEFT;
                break;
            case SDLK_RIGHT:
                keys_down ^= KEY_RIGHT;
                break;
            }
        }
    }
}

void update_position(Player *player, Map *map) {
    if (keys_down & KEY_UP) {
        player_move_forward(player, map, MOVE_SPEED);
    }
    if (keys_down & KEY_LEFT) {
        player_turn_left(player, TURN_SPEED);
    }
    if (keys_down & KEY_RIGHT) {
        player_turn_right(player, TURN_SPEED);
    }
}

void fill_pixel_buffer(Map *map, Player *player) {
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

    bool running = true;
    Map map = {0};
    map_load(&map, "tests/maps/map3.txt");
    Player player = player_create(&map);

    while (running) {
        int start = SDL_GetPerformanceCounter();

        handle_input(&running);
        update_position(&player, &map);
        fill_pixel_buffer(&map, &player);

        // update screen
        SDL_UpdateTexture(texture, NULL, frame_buffer, WIDTH * sizeof(int));
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        int end = SDL_GetPerformanceCounter();

        // cap frame rate
        double elapsed =
            (double)(end - start) / (double)SDL_GetPerformanceFrequency();
        if (elapsed < TARGET_FRAME_LEN) {
            SDL_Delay((int)((TARGET_FRAME_LEN - elapsed) * 1000.0));
        }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}