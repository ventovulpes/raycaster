#include "map.h"

#ifndef RAY_H
#define RAY_H

typedef struct Player Player;

typedef enum Direction {
    DIRECITON_X,
    DIRECTION_Y,
} Direction;

typedef struct Hit {
    double distance;
    Direction side;
    CellType object_type;
} Hit;

Hit cast_ray(Map *map, double x, double y, double direction);

void compute_hits(Hit hits[], Map *map, Player *player);

#endif