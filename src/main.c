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
    RED = 0xFFFF0000,
    DARK_RED = 0xFFA80000,
    GREEN = 0xFF00FF00,
    DARK_GREEN = 0xFF199E19,
    BLUE = 0xFF00F7FF,
    DARK_BLUE = 0xFF009EA3,
} Color;

// clang-format off
typedef enum {
    KEY_UP     = 0b0001,
    KEY_DOWN   = 0b0010,
    KEY_LEFT   = 0b0100,
    KEY_RIGHT  = 0b1000
} Key;
// clang-format on

#define CEILING_COLOR WHITE
#define FLOOR_COLOR BLACK

void put_pixel(int frame_buffer[], int x, int y, int color) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
        return;
    }
    frame_buffer[y * WIDTH + x] = color;
}

void clear(int frame_buffer[], int color) {
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        frame_buffer[i] = color;
    }
}

void put_vertical_slice(int frame_buffer[], int x, double distance, int color) {
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
        put_pixel(frame_buffer, x, i, pixel_color);
    }
}

bool handle_input(int *keys_down, double *mouse_movement) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            return false;
        } else if (event.type == SDL_EVENT_KEY_DOWN) {
            switch (event.key.key) {
            case SDLK_ESCAPE:
            case SDLK_Q:
                return false;
            case SDLK_UP:
            case SDLK_W:
                *keys_down |= KEY_UP;
                break;
            case SDLK_DOWN:
            case SDLK_S:
                *keys_down |= KEY_DOWN;
                break;
            case SDLK_LEFT:
            case SDLK_A:
                *keys_down |= KEY_LEFT;
                break;
            case SDLK_RIGHT:
            case SDLK_D:
                *keys_down |= KEY_RIGHT;
                break;
            }
        } else if (event.type == SDL_EVENT_KEY_UP) {
            switch (event.key.key) {
            case SDLK_UP:
            case SDLK_W:
                *keys_down ^= KEY_UP;
                break;
            case SDLK_DOWN:
            case SDLK_S:
                *keys_down ^= KEY_DOWN;
                break;
            case SDLK_LEFT:
            case SDLK_A:
                *keys_down ^= KEY_LEFT;
                break;
            case SDLK_RIGHT:
            case SDLK_D:
                *keys_down ^= KEY_RIGHT;
                break;
            }
        } else if (event.type == SDL_EVENT_MOUSE_MOTION) {
            *mouse_movement = event.motion.xrel;
        }
    }
    return true;
}

void update_position(Player *player, Map *map, int keys_down,
                     double mouse_movement) {
    if (keys_down & KEY_UP) {
        player_move_forward(player, map, MOVE_SPEED);
    }
    if (keys_down & KEY_DOWN) {
        player_move_backward(player, map, MOVE_SPEED);
    }
    if (keys_down & KEY_LEFT) {
        player_turn_left(player, TURN_SPEED);
    }
    if (keys_down & KEY_RIGHT) {
        player_turn_right(player, TURN_SPEED);
    }
    if (mouse_movement < 0) {
        player_turn_left(player, -mouse_movement * MOUSE_SENSITIVITY);
        mouse_movement = 0;
    } else if (mouse_movement > 0) {
        player_turn_right(player, mouse_movement * MOUSE_SENSITIVITY);
        mouse_movement = 0;
    }
}

void fill_pixel_buffer(int frame_buffer[], Hit hits[], Map *map,
                       Player *player) {
    compute_hits(hits, map, player);

    clear(frame_buffer, WHITE);
    for (int i = 0; i < WIDTH; i++) {
        Hit hit = hits[i];
        int color;
        switch (hit.object_type) {
        case RED_WALL:
            color = hit.side == DIRECITON_X ? DARK_RED : RED;
            break;
        case GREEN_WALL:
            color = hit.side == DIRECITON_X ? DARK_GREEN : GREEN;
            break;
        case BLUE_WALL:
            color = hit.side == DIRECITON_X ? DARK_BLUE : BLUE;
            break;
        default:
            color = WHITE;
        }
        put_vertical_slice(
            frame_buffer, i,
            hit.distance * cos(hit.direction - player->direction), color);
    }
}

int color_of_cell(const Map *map, size_t x, size_t y) {
    CellType object = map_get(map, x, y);
    switch (object) {
    case EMPTY:
        return BLACK;
    case RED_WALL:
        return RED;
    case GREEN_WALL:
        return GREEN;
    case BLUE_WALL:
        return BLUE;
    default:
        return WHITE;
    }
}

void fill_minimap(int frame_buffer[], const Map *map, const Player *player) {
    int x_offset = WIDTH - MINIMAP_MARGIN;
    int y_offset = MINIMAP_MARGIN;
    int width = map->width * MINIMAP_SIZE_MULT;
    int height = map->height * MINIMAP_SIZE_MULT;

    // draw map
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int x = (int)(i / MINIMAP_SIZE_MULT);
            int y = (int)(j / MINIMAP_SIZE_MULT);
            int color = color_of_cell(map, x, y);
            put_pixel(frame_buffer, x_offset - i, y_offset + j, color);
        }
    }

    // draw player
    int x = player->x * MINIMAP_SIZE_MULT;
    int y = player->y * MINIMAP_SIZE_MULT;
    int player_size = 3;
    for (int i = 0; i < player_size; i++) {
        for (int j = 0; j < player_size; j++) {
            if (i < 0 || j < 0 || i >= WIDTH || j >= HEIGHT)
                continue;
            put_pixel(frame_buffer, x_offset - x + i, y_offset + y + j, WHITE);
        }
    }
}

int main(int argc, char **argv) {
    const char *file_name;
    if (argc == 1) {
        file_name = "maps/default.txt";
    } else {
        file_name = argv[1];
    }

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
    SDL_SetWindowRelativeMouseMode(window, true);

    int frame_buffer[WIDTH * HEIGHT];
    Hit hits[WIDTH];
    // bitmap which stores which keys are being pressed
    int keys_down = 0;
    double mouse_movement = 0;

    bool running = true;
    Map map = {0};
    map_load(&map, file_name);
    Player player = player_create(&map);

    while (running) {
        int start = SDL_GetPerformanceCounter();

        // input
        running = handle_input(&keys_down, &mouse_movement);
        update_position(&player, &map, keys_down, mouse_movement);
        mouse_movement = 0;

        // fill buffer
        fill_pixel_buffer(frame_buffer, hits, &map, &player);
        fill_minimap(frame_buffer, &map, &player);

        // update screen
        SDL_UpdateTexture(texture, NULL, frame_buffer, WIDTH * sizeof(int));
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        int end = SDL_GetPerformanceCounter();

        // cap frame rate to keep speeds constant
        double elapsed =
            (double)(end - start) / (double)SDL_GetPerformanceFrequency();
        if (elapsed < TARGET_FRAME_LEN) {
            SDL_Delay((int)((TARGET_FRAME_LEN - elapsed) * 1000.0));
        }
    }

    SDL_SetWindowRelativeMouseMode(window, false);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}