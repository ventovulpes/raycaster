#include <stdbool.h>
#include <stdlib.h>

#ifndef MAP_H
#define MAP_H

typedef enum {
    EMPTY,
    WALL,
    PLAYER,
} CellType;

typedef struct Map {
    size_t height;
    size_t width;
    CellType *cells;
} Map;

bool map_load(Map *map, const char *file_name);

void map_destroy(Map *map);

CellType map_get(Map *map, size_t x, size_t y);

#endif