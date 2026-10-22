#include "config.h"
#include "map.h"
#include "player.h"
#include "ray.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define TEST(fn) {#fn, fn}

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
            case RED_WALL:
                ch = 'R';
                break;
            case GREEN_WALL:
                ch = 'G';
                break;
            case BLUE_WALL:
                ch = 'B';
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
// `x = 1`
// `y = 1`
// `direction = 0`
static Player make_test_player(void) {
    return (Player){
        .x = 1,
        .y = 1,
        .direction = 0,
    };
}

// 22x22 map with player at `(10, 10)`
static Map make_test_map(void) {
    Map m = {0};
    map_load(&m, "tests/maps/large.txt");
    return m;
}

static bool test_player_create(void) {
    Map m = {0};
    map_load(&m, "tests/maps/map1.txt");

    Player p = player_create(&m);
    Player expected = {
        .x = 1.5,
        .y = 3.5,
        .direction = 0,
    };

    bool res = true;
    for (size_t i = 0; i < m.width * m.height; i++) {
        if (m.cells[i] == PLAYER) {
            res = false;
        }
    }
    map_destroy(&m);
    return assert_player_eq(&p, &expected) && res;
}

static bool test_player_move_forward(void) {
    Player p = make_test_player();
    Map m = make_test_map();
    player_move_forward(&p, &m, 10);

    Player expected = {
        .x = 11,
        .y = 1,
        .direction = 0,
    };
    map_destroy(&m);
    return assert_player_eq(&p, &expected);
}

static bool test_player_move_backward(void) {
    Player p = make_test_player();
    Map m = make_test_map();
    player_turn_left(&p, 180);
    player_move_backward(&p, &m, 10);

    Player expected = {
        .x = 11,
        .y = 1,
        .direction = PI,
    };
    map_destroy(&m);
    return assert_player_eq(&p, &expected);
}

static bool test_player_turn_left_and_move_forward(void) {
    Map m = make_test_map();
    Player p = player_create(&m);
    player_turn_left(&p, 45);
    player_move_forward(&p, &m, 1);

    Player expected = {
        .x = 10.5 + sqrt(2) / 2.0,
        .y = 10.5 + sqrt(2) / 2.0,
        .direction = 7.0 * PI / 4.0,
    };
    map_destroy(&m);
    return assert_player_eq(&p, &expected);
}

static bool test_player_turn_right_and_move_forward(void) {
    Map m = make_test_map();
    Player p = player_create(&m);
    player_turn_right(&p, 45);
    player_move_forward(&p, &m, 1);

    Player expected = {
        .x = 10.5 + sqrt(2) / 2.0,
        .y = 10.5 - sqrt(2) / 2.0,
        .direction = PI / 4.0,
    };
    map_destroy(&m);
    return assert_player_eq(&p, &expected);
}

static bool test_player_turn_around_and_move_forward(void) {
    Player p = make_test_player();
    Map m = make_test_map();
    player_turn_right(&p, 180);
    player_move_forward(&p, &m, 1);

    Player expected = {
        .x = 1 + MIN_WALL_DISTANCE,
        .y = 1,
        .direction = PI,
    };
    map_destroy(&m);
    return assert_player_eq(&p, &expected);
}

static bool test_player_collide_with_wall_east(void) {
    Map m = make_test_map();
    Player p = player_create(&m);
    player_move_forward(&p, &m, 20);

    Player expected = {
        .x = 21 - MIN_WALL_DISTANCE,
        .y = 10.5,
        .direction = 0,
    };
    map_destroy(&m);
    return assert_player_eq(&p, &expected);
}

static bool test_player_collide_with_wall_north(void) {
    Map m = make_test_map();
    Player p = player_create(&m);
    player_turn_right(&p, 90);
    player_move_forward(&p, &m, 20);

    Player expected = {
        .x = 10.5,
        .y = 1 + MIN_WALL_DISTANCE,
        .direction = PI / 2.0,
    };
    map_destroy(&m);
    return assert_player_eq(&p, &expected);
}

static bool test_player_collide_with_wall_west(void) {
    Map m = make_test_map();
    Player p = player_create(&m);
    player_turn_right(&p, 180);
    player_move_forward(&p, &m, 20);

    Player expected = {
        .x = 1 + MIN_WALL_DISTANCE,
        .y = 10.5,
        .direction = PI,
    };
    map_destroy(&m);
    return assert_player_eq(&p, &expected);
}

static bool test_player_collide_with_wall_south(void) {
    Map m = make_test_map();
    Player p = player_create(&m);
    player_turn_left(&p, 90);
    player_move_forward(&p, &m, 20);

    Player expected = {
        .x = 10.5,
        .y = 21 - MIN_WALL_DISTANCE,
        .direction = 3.0 * PI / 2.0,
    };
    map_destroy(&m);
    return assert_player_eq(&p, &expected);
}

static bool test_map_load(void) {
    Map m = {0};
    // clang-format off
    CellType expected[6 * 12] = {
        GREEN_WALL, GREEN_WALL, GREEN_WALL, GREEN_WALL, GREEN_WALL, GREEN_WALL, GREEN_WALL, GREEN_WALL, GREEN_WALL, GREEN_WALL, GREEN_WALL, GREEN_WALL, 
        GREEN_WALL, EMPTY, EMPTY, GREEN_WALL, GREEN_WALL, GREEN_WALL, EMPTY, EMPTY, EMPTY, EMPTY, GREEN_WALL, GREEN_WALL, 
        GREEN_WALL, GREEN_WALL, EMPTY, EMPTY, EMPTY, EMPTY, GREEN_WALL, GREEN_WALL, EMPTY, EMPTY, GREEN_WALL, GREEN_WALL, 
        GREEN_WALL, PLAYER, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, GREEN_WALL, 
        GREEN_WALL, GREEN_WALL, GREEN_WALL, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, GREEN_WALL, GREEN_WALL,
        GREEN_WALL, GREEN_WALL, GREEN_WALL, GREEN_WALL, GREEN_WALL, GREEN_WALL, GREEN_WALL, GREEN_WALL, GREEN_WALL, GREEN_WALL, GREEN_WALL, GREEN_WALL, 
    };
    // clang-format off
    bool res = map_load(&m, "tests/maps/map1.txt") && assert_cell_array_eq(m.cells, expected, 6, 12);
    map_destroy(&m);
    return res;
}

static bool test_map_init_with_no_player(void) {
    Map m = {0};
    return !map_load(&m, "tests/maps/no_player.txt");
}

static bool test_map_init_with_multiple_players(void) {
    Map m = {0};
    return !map_load(&m, "tests/maps/multiple_players.txt");
}

static bool test_ray_cast_distance_with_int_distance(void) {
    Map m = make_test_map();
    bool res = assert_eq(cast_ray(&m, 2, 2, 0).distance, 19.0);
    map_destroy(&m);
    return res;
}

static bool test_ray_cast_distance_with_non_int_distance(void) {
    Map m = make_test_map();
    bool res = assert_eq(cast_ray(&m, 2.1, 2, 0).distance, 18.9);
    map_destroy(&m);
    return res;
}

static bool test_ray_cast_distance_towards_left(void) {
    Map m = make_test_map();
    bool res = assert_eq(cast_ray(&m, 2, 2, PI).distance, 1);
    map_destroy(&m);
    return res;
}

static bool test_ray_cast_distance_towards_bottom(void) {
    Map m = make_test_map();
    bool res = assert_eq(cast_ray(&m, 2, 2, 1.5 * PI).distance, 19.0);
    map_destroy(&m);
    return res;
}

static bool test_ray_cast_distance_at_angle(void) {
    Map m = make_test_map();
    bool res = assert_eq(cast_ray(&m, 2, 2, PI / 4).distance, sqrt(2));
    map_destroy(&m);
    return res;
}

static bool test_ray_cast_distance_at_large_angle(void) {
    Map m = make_test_map();
    bool res = assert_eq(cast_ray(&m, 2, 2, 4.66).distance, sqrt(1 + pow(19, 2)));
    map_destroy(&m);
    return res;
}


int main(void) {
    printf("Running tests...\n");

    Test tests[] = {
        TEST(test_player_move_forward),
        TEST(test_player_move_backward),
        TEST(test_player_turn_left_and_move_forward),
        TEST(test_player_turn_right_and_move_forward),
        TEST(test_player_turn_around_and_move_forward),
        TEST(test_map_load),
        TEST(test_map_init_with_no_player),
        TEST(test_map_init_with_multiple_players),
        TEST(test_player_create),
        TEST(test_ray_cast_distance_with_int_distance),
        TEST(test_ray_cast_distance_with_non_int_distance),
        TEST(test_ray_cast_distance_towards_left),
        TEST(test_ray_cast_distance_towards_bottom),
        TEST(test_ray_cast_distance_at_angle),
        TEST(test_ray_cast_distance_at_large_angle),
        TEST(test_player_collide_with_wall_east),
        TEST(test_player_collide_with_wall_north),
        TEST(test_player_collide_with_wall_west),
        TEST(test_player_collide_with_wall_south),
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
        printf("\n\033[32mAll %d tests passed!\033[0m\n", total);
    } else {
        printf("\n%d/%d tests passed", passed, total);
    }
}