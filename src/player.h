#ifndef PLAYER_H
#define PLAYER_H

#define PI 3.14159265358979323846

typedef struct {
    double x;
    double y;
    double direction;
} Player;

void player_move_forward(Player *player, double distance);

void player_turn_left(Player *player, double deg);

void player_turn_right(Player *player, double deg);

#endif