#include "ray.h"
#include "config.h"
#include "map.h"
#include "math.h"
#include "player.h"
#include <stdio.h>

static bool approx_eq(double a, double b) { return fabs(a - b) < EPSILON; }

// Returns the `Hit` from casting a ray starting at `(x, y)` with `direction`
// (in radians).
Hit cast_ray(const Map *map, double x, double y, double direction) {
    // calculates distance to nearest obstable using the DDA algorithm

    double d_x = cos(direction);
    double d_y = -sin(direction);

    int cell_x = floor(x);
    int cell_y = floor(y);

    // compute distance to first x and y grid intersections
    double x_dist, y_dist;
    if (approx_eq(d_x, 0)) {
        x_dist = INFINITY;
    } else if (d_x > 0) {
        x_dist = (floor(x) + 1 - x) / d_x;
    } else if (d_x < 0) {
        x_dist = (floor(x) - x) / d_x;
    }
    if (approx_eq(d_y, 0)) {
        y_dist = INFINITY;
    } else if (d_y > 0) {
        y_dist = (floor(y) + 1 - y) / d_y;
    } else if (d_y < 0) {
        y_dist = (floor(y) - y) / d_y;
    }

    // compute distance that must be travelled along ray to reach the next
    // intersection for x and y
    double step_x = !approx_eq(d_x, 0) ? 1 / fabs(d_x) : INFINITY;
    double step_y = !approx_eq(d_y, 0) ? 1 / fabs(d_y) : INFINITY;

    // calculate the direction the current cell should advance
    double cell_step_x = d_x > 0 ? 1 : -1;
    double cell_step_y = d_y > 0 ? 1 : -1;
    double cur_dist = 0;

    Direction side;

    while (map_get(map, cell_x, cell_y) == EMPTY) {
        // advance in the direction which has the shortest distance
        if (x_dist < y_dist) {
            cell_x += cell_step_x;
            cur_dist = x_dist;
            x_dist += step_x;
            side = DIRECITON_X;
        } else {
            cell_y += cell_step_y;
            cur_dist = y_dist;
            y_dist += step_y;
            side = DIRECTION_Y;
        }
    }
    CellType object_type = map_get(map, cell_x, cell_y);

    return (Hit){
        cur_dist,
        direction,
        side,
        object_type,
    };
}

// Sends `WIDTH` number of rays inside `FOV`, populating `hits` with hit data.
void compute_hits(Hit hits_out[], const Map *map, const Player *player) {
    for (int i = 0; i < WIDTH; i++) {
        double screen_x = i - WIDTH / 2;
        double angle = atan2(screen_x, FOV_DIST) + player->direction;
        hits_out[i] = cast_ray(map, player->x, player->y, angle);
    }
}