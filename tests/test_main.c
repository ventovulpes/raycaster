#include "map.h"
#include "player.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define TEST(fn) {#fn, fn}
#define EPSILON 0.000001

typedef struct {
    const char *name;
    bool (*fn)(void);
} Test;

static void print_player(Player *p) {
    printf("Player { x: %f, y: %f, direction: %f}", p->x, p->y, p->direction);
}

static void print_cells(CellType *cells, size_t height, size_t width) {
    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            char ch;
            switch (cells[i * width + j]) {
            case EMPTY:
                ch = '.';
                break;
            case WALL:
                ch = 'X';
                break;
            case PLAYER:
                ch = 'P';
                break;
            }
            printf("%c ", ch);
        }
        printf("\n");
    }
}

static bool assert_eq(double actual, double expected) {
    if (fabs(actual - expected) < EPSILON) {
        return true;
    }
    printf("\033[31mTest failed:\nactual:   %f\nexpected: %f\033[0m\n", actual,
           expected);
    return false;
}

static bool assert_player_eq(Player *actual, Player *expected) {
    bool res = true;

    res &= assert_eq(actual->x, expected->x);
    res &= assert_eq(actual->y, expected->y);
    res &= assert_eq(actual->direction, expected->direction);

    if (res) {
        return true;
    }

    printf("\033[31mTest failed:\nactual:   ");
    print_player(actual);
    printf("\nexpected: ");
    print_player(expected);
    printf("\033[0m\n");
    return false;
}

static bool assert_cell_array_eq(CellType *actual, CellType *expected,
                                 size_t height, size_t width) {
    bool res = true;

    for (size_t i = 0; i < height * width; i++) {
        if (actual[i] != expected[i]) {
            res = false;
        }
    }

    if (!res) {
        printf("\033[31mTest failed:\nactual:\n");
        print_cells(actual, height, width);
        printf("\nexpected:\n");
        print_cells(expected, height, width);
        printf("\033[0m\n");

        return false;
    }

    return res;
}

static Player make_player(void) {
    return (Player){
        .x = 0,
        .y = 0,
        .direction = 0,
    };
}

static bool test_player_move_forward(void) {
    Player p = make_player();
    player_move_forward(&p, 10);

    Player expected = {
        .x = 10,
        .y = 0,
        .direction = 0,
    };
    return assert_player_eq(&p, &expected);
}

static bool test_player_turn_left_and_move_forward(void) {
    Player p = make_player();
    player_turn_left(&p, 45);
    player_move_forward(&p, 1);

    Player expected = {
        .x = sqrt(2) / 2.0,
        .y = sqrt(2) / 2.0,
        .direction = PI / 4.0,
    };
    return assert_player_eq(&p, &expected);
}

static bool test_player_turn_right_and_move_forward(void) {
    Player p = make_player();
    player_turn_right(&p, 45);
    player_move_forward(&p, 1);

    Player expected = {
        .x = sqrt(2) / 2.0,
        .y = -sqrt(2) / 2.0,
        .direction = 7.0 * PI / 4.0,
    };
    return assert_player_eq(&p, &expected);
}

static bool test_player_turn_around_and_move_forward(void) {
    Player p = make_player();
    player_turn_right(&p, 180);
    player_move_forward(&p, 10);

    Player expected = {
        .x = -10,
        .y = 0,
        .direction = PI,
    };
    return assert_player_eq(&p, &expected);
}

static bool test_map_calc_dimensions(void) {
    size_t height = 0;
    size_t width = 0;
    return map_calc_dimensions("maps/map1.txt", &height, &width) &&
           assert_eq(height, 6) && assert_eq(width, 12);
}

static bool test_map_calc_dimensions_with_invalid_chars(void) {
    size_t height = 0;
    size_t width = 0;
    return !map_calc_dimensions("maps/invalid_chars.txt", &height, &width);
}

static bool test_map_calc_dimensions_with_ragged_map(void) {
    size_t height = 0;
    size_t width = 0;
    return map_calc_dimensions("maps/ragged.txt", &height, &width) &&
           assert_eq(height, 5) && assert_eq(width, 6);
}

static bool test_map_calc_dimensions_with_small_map(void) {
    size_t height = 0;
    size_t width = 0;
    return map_calc_dimensions("maps/small.txt", &height, &width) &&
           assert_eq(height, 3) && assert_eq(width, 3);
}

static bool test_map_init(void) {
    size_t width = 6;
    size_t height = 12;
    Map map = map_create(width, height);
    // clang-format off
    CellType expected[6 * 12] = {
        WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, 
        WALL, EMPTY, EMPTY, WALL, WALL, WALL, EMPTY, EMPTY, EMPTY, EMPTY, WALL, WALL, 
        WALL, WALL, EMPTY, EMPTY, EMPTY, EMPTY, WALL, WALL, EMPTY, EMPTY, WALL, WALL, 
        WALL, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL, 
        WALL, WALL, WALL, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL, WALL,
        WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, 
    };
    // clang-format off
    map_data_init(map.data, "maps/map1.txt", width, height);
    return assert_cell_array_eq(map.data, expected, width, height);
}

int main(void) {
    printf("Running tests...\n");

    Test tests[] = {
        TEST(test_player_move_forward),
        TEST(test_player_turn_left_and_move_forward),
        TEST(test_player_turn_right_and_move_forward),
        TEST(test_player_turn_around_and_move_forward),
        TEST(test_map_calc_dimensions),
        TEST(test_map_calc_dimensions_with_invalid_chars),
        TEST(test_map_calc_dimensions_with_ragged_map),
        TEST(test_map_calc_dimensions_with_small_map),
        TEST(test_map_init),
    };

    int passed = 0;
    int total = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < total; i++) {
        printf("%s\n", tests[i].name);
        if (tests[i].fn()) {
            passed++;
        }
    }

    if (passed == total) {
        printf("\n\033[32mAll tests passed!\033[0m\n");
    } else {
        printf("\n%d/%d tests passed", passed, total);
    }
}