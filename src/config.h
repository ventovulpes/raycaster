#ifndef CONFIG_H
#define CONFIG_H

#define EPSILON 0.001
#define PI 3.14159265358979323846

#define WIDTH 800
#define HEIGHT 600

#define MINIMAP_SIZE_MULT 10
#define MINIMAP_MARGIN 10

#define FOV 70
#define FOV_RAD (FOV * PI / 180.0)
#define FOV_DIST (0.5 * WIDTH / tan(0.5 * FOV_RAD))

#define MIN_WALL_DISTANCE 0.3

#define MOVE_SPEED 1.5
#define TURN_SPEED 45.0
#define MOUSE_SENSITIVITY 7

#endif