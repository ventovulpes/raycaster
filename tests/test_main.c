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

// Player with:
// `x = 0`
// `y = 0`
// `direction = 0`
static Player make_test_player(void) {
    return (Player){
        .x = 0,
        .y = 0,
        .direction = 0,
    };
}

// 22x22 map with player at `(10, 10)`
static Map make_test_map(void) {
    Map m;
    map_load(&m, "tests/maps/large.txt");
    return m;
}

static bool test_player_create(void) {
    Map map;
    map_load(&map, "tests/maps/map1.txt");

    Player p = player_create(&map);
    Player expected = {
        .x = 1,
        .y = 3,
        .direction = 0,
    };
    return assert_player_eq(&p, &expected);
}

static bool test_player_move_forward(void) {
    Player p = make_test_player();
    Map m = make_test_map();
    player_move_forward(&p, &m, 10);

    Player expected = {
        .x = 10,
        .y = 0,
        .direction = 0,
    };
    return assert_player_eq(&p, &expected);
}

static bool test_player_turn_left_and_move_forward(void) {
    Player p = make_test_player();
    Map m = make_test_map();
    player_turn_left(&p, 45);
    player_move_forward(&p, &m, 1);

    Player expected = {
        .x = sqrt(2) / 2.0,
        .y = sqrt(2) / 2.0,
        .direction = PI / 4.0,
    };
    return assert_player_eq(&p, &expected);
}

static bool test_player_turn_right_and_move_forward(void) {
    Player p = make_test_player();
    Map m = make_test_map();
    player_turn_right(&p, 45);
    player_move_forward(&p, &m, 1);

    Player expected = {
        .x = sqrt(2) / 2.0,
        .y = -sqrt(2) / 2.0,
        .direction = 7.0 * PI / 4.0,
    };
    return assert_player_eq(&p, &expected);
}

static bool test_player_turn_around_and_move_forward(void) {
    Player p = make_test_player();
    Map m = make_test_map();
    player_turn_right(&p, 180);
    player_move_forward(&p, &m, 1);

    Player expected = {
        .x = -1,
        .y = 0,
        .direction = PI,
    };
    return assert_player_eq(&p, &expected);
}

static bool test_map_load(void) {
    Map map;
    // clang-format off
    CellType expected[6 * 12] = {
        WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, 
        WALL, EMPTY, EMPTY, WALL, WALL, WALL, EMPTY, EMPTY, EMPTY, EMPTY, WALL, WALL, 
        WALL, WALL, EMPTY, EMPTY, EMPTY, EMPTY, WALL, WALL, EMPTY, EMPTY, WALL, WALL, 
        WALL, PLAYER, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL, 
        WALL, WALL, WALL, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL, WALL,
        WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, 
    };
    // clang-format off
    return map_load(&map, "tests/maps/map1.txt") && assert_cell_array_eq(map.cells, expected, 6, 12);
}

static bool test_map_init_with_no_player(void) {
    Map m;
    return !map_load(&m, "tests/maps/no_player.txt");
}

static bool test_map_init_with_multiple_players(void) {
    Map m;
    return !map_load(&m, "tests/maps/multiple_players.txt");
}

int main(void) {
    printf("Running tests...\n");

    Test tests[] = {
        TEST(test_player_move_forward),
        TEST(test_player_turn_left_and_move_forward),
        TEST(test_player_turn_right_and_move_forward),
        TEST(test_player_turn_around_and_move_forward),
        TEST(test_map_load),
        TEST(test_map_init_with_no_player),
        TEST(test_map_init_with_multiple_players),
        TEST(test_player_create),
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