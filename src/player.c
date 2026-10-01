#include "player.h"
#include "config.h"
#include "map.h"
#include "ray.h"
#include <math.h>
#include <stdlib.h>

// Creates a `Player` with coordinates of `PLAYER` in the given map and
// `direction = 0`. If there is no `PLAYER` in the given map, sets the `Player`
// coordinates to first empty cell.
Player player_create(Map *map) {
    size_t x, y;
    bool found_empty = false;
    for (size_t i = 0; i < map->height; i++) {
        for (size_t j = 0; j < map->width; j++) {
            CellType cell = map->cells[i * map->width + j];
            if (cell == PLAYER) {
                x = j;
                y = i;
                goto found_player;
            } else if (!found_empty && cell == EMPTY) {
                found_empty = true;
                x = j;
                y = i;
            }
        }
    }
found_player:
    map->cells[y * map->width + x] = EMPTY;
    return (Player){
        .x = (double)x + 0.5,
        .y = (double)y + 0.5,
        .direction = 0.0,
    };
}

void player_move_forward(Player *player, Map *map, double distance) {
    double distance_before_obstacle =
        cast_ray(map, player->x, player->y, player->direction).distance;
    if (distance > distance_before_obstacle - MIN_WALL_DISTANCE) {
        distance = distance_before_obstacle - MIN_WALL_DISTANCE;
    }

    double d_x = cos(player->direction) * distance;
    double d_y = -sin(player->direction) * distance;

    player->x += d_x;
    player->y += d_y;
}

static double mod(double x, double m) { return fmod(fmod(x, m) + m, m); }

void player_turn_left(Player *player, double deg) {
    double rad = deg * (PI / 180.0);
    player->direction = mod(player->direction - rad, 2.0 * PI);
}

void player_turn_right(Player *player, double deg) {
    double rad = deg * (PI / 180.0);
    player->direction = mod(player->direction + rad, 2.0 * PI);
}