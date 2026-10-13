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
    double direction;
    Direction side;
    CellType object_type;
} Hit;

Hit cast_ray(const Map *map, double x, double y, double direction);

void compute_hits(Hit hits_out[], const Map *map, const Player *player);

#endif