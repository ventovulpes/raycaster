#include "ray.h"
#include "map.h"
#include "math.h"

#define RAY_STEP 0.001

// Returns the distance to the nearest non empty cell by casting a ray starting
// at `(x, y)` with `direction` (in radians).
double ray_cast_distance(Map *map, double x, double y, double direction) {
    double d_x = cos(direction) * RAY_STEP;
    double d_y = -sin(direction) * RAY_STEP;

    double cur_x = x;
    double cur_y = y;

    while (map_get(map, floor(cur_x), floor(cur_y)) == EMPTY) {
        cur_x += d_x;
        cur_y += d_y;
    }

    return sqrt(pow(cur_x - x, 2.0) + pow(cur_y - y, 2.0));
}