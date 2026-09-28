#include "ray.h"
#include "math.h"
#include <stdio.h>

#define EPSILON 0.00001

static bool approx_eq(double a, double b) { return fabs(a - b) < EPSILON; }

// Returns the distance to the nearest non empty cell by casting a ray starting
// at `(x, y)` with `direction` (in radians).
double ray_cast_distance(Map *map, double x, double y, double direction) {
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

    while (map_get(map, cell_x, cell_y) == EMPTY) {
        // advance in the direction which has the shortest distance
        if (x_dist < y_dist) {
            cell_x += cell_step_x;
            cur_dist = x_dist;
            x_dist += step_x;
        } else {
            cell_y += cell_step_y;
            cur_dist = y_dist;
            y_dist += step_y;
        }
    }

    return cur_dist;
}