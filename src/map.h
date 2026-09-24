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

Map map_create(size_t height, size_t width);

bool map_calc_dimensions(char *file_name, size_t *height, size_t *width);

bool map_data_init(CellType *cells, char *file_name, size_t height,
                   size_t width);

#endif