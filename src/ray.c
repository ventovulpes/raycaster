#include "ray.h"
#include "map.h"
#include "math.h"

#define RAY_STEP 0.01

// Returns the distance to the nearest non empty cell by casting a ray starting
// at `(x, y)` with `direction` (in degrees).
double ray_cast_distance(Map *map, double x, double y, double direction) {
    double d_x = cos(direction * (PI / 180.0)) * RAY_STEP;
    double d_y = -sin(direction * (PI / 180.0)) * RAY_STEP;

    double (*round_x)(double) = d_x > 0 ? floor : ceil;
    double (*round_y)(double) = d_y > 0 ? floor : ceil;

    double cur_x = x;
    double cur_y = y;

    while (map_get(map, floor(cur_x), floor(cur_y)) == EMPTY) {
        cur_x += d_x;
        cur_y += d_y;
    }

    return sqrt(pow(cur_x - x, 2.0) + pow(cur_y - y, 2.0));
}