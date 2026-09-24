#include "player.h"
#include "map.h"
#include <math.h>
#include <stdlib.h>

// Creates a `Player` with coordinates of `PLAYER` in the given map and
// `direction = 0`. If there is no `PLAYER` in the given map, sets the `Player`
// coordinates to first empty cell.
Player player_create(Map *map) {
    double x, y;
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
    return (Player){
        .x = x,
        .y = y,
        .direction = 0,
    };
}

void player_move_forward(Player *player, double distance) {
    player->x += cos(player->direction) * distance;
    player->y += sin(player->direction) * distance;
}

static double mod(double x, double m) { return fmod(fmod(x, m) + m, m); }

void player_turn_left(Player *player, double deg) {
    double rad = deg * (PI / 180.0);
    player->direction = mod(player->direction + rad, 2.0 * PI);
}

void player_turn_right(Player *player, double deg) {
    double rad = deg * (PI / 180.0);
    player->direction = mod(player->direction - rad, 2.0 * PI);
}