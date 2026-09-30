#ifndef CONFIG_H
#define CONFIG_H

#define EPSILON 0.001
#define PI 3.14159265358979323846

#define WIDTH 800
#define HEIGHT 600

#define FOV 70
#define FOV_RAD (FOV * PI / 180.0)

#define TARGET_FPS 60.0
#define TARGET_FRAME_LEN 1.0 / TARGET_FPS

#define MIN_WALL_DISTANCE 0.3

#endif