#include <stdbool.h>
#include <stdlib.h>

#ifndef MAP_H
#define MAP_H

typedef enum {
    EMPTY,
    WALL,
    PLAYER,
} CellType;

typedef struct {
    size_t height;
    size_t width;
    CellType *cells;
} Map;

bool map_load(Map *map, char *file_name);

CellType map_get(Map *map, size_t x, size_t y);

#endif