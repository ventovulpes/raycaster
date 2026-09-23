#include "player.h"
#include <math.h>

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