#ifndef PLAYER_H
#define PLAYER_H

#include "map.h"

#define PI 3.14159265358979323846

typedef struct {
    double x;
    double y;
    double direction;
} Player;

Player player_create(Map *map);

void player_move_forward(Player *player, double distance);

void player_turn_left(Player *player, double deg);

void player_turn_right(Player *player, double deg);

#endif