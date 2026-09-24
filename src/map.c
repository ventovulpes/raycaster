#include "map.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 256
#define VALID_CHARS "X.P"

// Creates a Map with dimensions `height` and `width`.
Map map_create(size_t height, size_t width) {
    Map map = {
        .height = height,
        .width = width,
        .cells = (CellType *)malloc(height * width * sizeof *map.cells),
    };

    return map;
}

// Calculates the dimensions needed for map in `file_name`. Updates `height` and
// `width` in place.
bool map_calc_dimensions(char *file_name, size_t *height, size_t *width) {
    FILE *f = fopen(file_name, "r");
    if (f == NULL) {
        perror("error reading file");
        return false;
    }

    char line[BUFFER_SIZE];
    *width = 0;
    *height = 0;
    while (fgets(line, BUFFER_SIZE, f) != NULL) {
        char *ch = line;
        size_t i = 0;
        while (*ch != '\0' && *ch != '\n') {
            if (strchr(VALID_CHARS, *ch) == NULL) {
                perror("file contains invalid character");
                fclose(f);
                return false;
            }
            i++;
            ch++;
        }

        // `width` is set to line with max width
        if (i > *width) {
            *width = i;
        }
        if (i > 0) {
            (*height)++;
        }
    }

    // for added walls on all sides
    *width += 2;
    *height += 2;

    fclose(f);
    return true;
}

// Initializes `map`'s cells based on the given file.
bool map_data_init(CellType *cells, char *file_name, size_t height,
                   size_t width) {
    FILE *f = fopen(file_name, "r");
    if (f == NULL) {
        perror("error reading file");
        return false;
    }

    for (size_t i = 0; i < width; i++) {
        cells[i] = WALL;
    }

    bool found_player = false;
    for (size_t i = 1; i < height - 1; i++) {
        char line[BUFFER_SIZE];
        fgets(line, BUFFER_SIZE, f);
        char *ch = line;

        for (size_t j = 0; j < width; j++) {
            // add surrounding walls
            if (j == 0 || j == width - 1) {
                cells[width * i + j] = WALL;
                continue;
            }

            CellType cell;
            switch (*ch) {
            case '.':
                cell = EMPTY;
                break;
            case 'X':
                cell = WALL;
                break;
            case 'P':
                if (found_player) {
                    perror(
                        "cannot initialize map with more than one P character");
                    fclose(f);
                    return false;
                }
                found_player = true;
                cell = PLAYER;
                break;
            }

            cells[width * i + j] = cell;
            ch++;
        }
    }

    for (size_t i = 0; i < width; i++) {
        cells[width * (height - 1) + i] = WALL;
    }

    fclose(f);

    if (!found_player) {
        perror("file must contain one P character to initialize map");
        return false;
    }
    return true;
}