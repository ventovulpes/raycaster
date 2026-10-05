#ifndef PLAYER_H
#define PLAYER_H

typedef struct Player {
    double x;
    double y;
    double direction;
} Player;

typedef struct Map Map;

Player player_create(Map *map);

void player_move_forward(Player *player, Map *map, double distance);

void player_move_backward(Player *player, Map *map, double distance);

void player_turn_left(Player *player, double deg);

void player_turn_right(Player *player, double deg);

#endif