#include "player.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define EPSILON 0.000001

static void print_player(Player *p) {
    printf("Player { x: %f, y: %f, direction: %f}", p->x, p->y, p->direction);
}

static bool assert_eq(double a, double b) { return fabs(a - b) < EPSILON; }

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

static Player make_player(void) {
    return (Player){
        .x = 0,
        .y = 0,
        .direction = 0,
    };
}

static bool test_player_move_forward(void) {
    printf("%s\n", __func__);

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
    printf("%s\n", __func__);

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
    printf("%s\n", __func__);

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
    printf("%s\n", __func__);

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

int main(void) {
    printf("Running tests...\n");

    bool (*tests[])(void) = {
        test_player_move_forward,
        test_player_turn_left_and_move_forward,
        test_player_turn_right_and_move_forward,
        test_player_turn_around_and_move_forward,
    };

    int passed = 0;
    int total = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < total; i++) {
        if (tests[i]()) {
            passed++;
        }
    }

    if (passed == total) {
        printf("\n\033[32mAll tests passed!\033[0m\n");
    } else {
        printf("\n%d/%d tests passed", passed, total);
    }
}