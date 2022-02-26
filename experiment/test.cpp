#include <cassert>
#include "pazusoba.h"

int main() {
    printf("test set_board\n");
    auto solver = pazusoba::solver();
    solver.set_board("DGRRBLHGBBGGRDDDDLBGHDBLLHDBLD");
    assert(solver.board_size() == 30);
    assert(solver.row() == 5);
    assert(solver.column() == 6);
    assert(solver.max_combo() == 8);
    assert(solver.min_erase() == 3);
    assert(solver.get_board_string(solver.board()) ==
           "DGRRBLHGBBGGRDDDDLBGHDBLLHDBLD");
    printf("test set_board passed\n");
    printf("====================================\n");
    printf("test expand\n");
    std::vector<pazusoba::state> next_states;
    next_states.resize(4);
    // top left corner
    pazusoba::state top_left;
    top_left.curr = 0;
    top_left.prev = 0;
    top_left.begin = 0;
    solver.expand(solver.board(), top_left, next_states, 0);
    int valid = 0;
    for (const auto& s : next_states) {
        if (s.score != MIN_STATE_SCORE) {
            printf("initial %d, %d -> %d\n", s.begin, s.prev, s.curr);
            solver.print_board(s.board);
            solver.print_route(s.route, 1, 0);
            assert(s.curr == 6 || s.curr == 1);
            valid++;
        }
    }
    assert(valid == 2);
    next_states.clear();
    next_states.resize(4);

    // bottom left corner
    pazusoba::state bottom_left;
    bottom_left.curr = 24;
    bottom_left.prev = 24;
    bottom_left.begin = 24;
    solver.expand(solver.board(), bottom_left, next_states, 0);
    valid = 0;
    for (const auto& s : next_states) {
        if (s.score != MIN_STATE_SCORE) {
            printf("initial %d, %d -> %d\n", s.begin, s.prev, s.curr);
            solver.print_board(s.board);
            assert(s.curr == 18 || s.curr == 25);
            valid++;
        }
    }
    assert(valid == 2);
    next_states.clear();
    next_states.resize(4);

    // top right corner
    pazusoba::state top_right;
    top_right.curr = 5;
    top_right.prev = 5;
    top_right.begin = 5;
    solver.expand(solver.board(), top_right, next_states, 0);
    valid = 0;
    for (const auto& s : next_states) {
        if (s.score != MIN_STATE_SCORE) {
            printf("initial %d, %d -> %d\n", s.begin, s.prev, s.curr);
            solver.print_board(s.board);
            assert(s.curr == 4 || s.curr == 11);
            valid++;
        }
    }
    assert(valid == 2);
    next_states.clear();
    next_states.resize(4);

    // bottom right corner
    pazusoba::state bottom_right;
    bottom_right.curr = 29;
    bottom_right.prev = 29;
    bottom_right.begin = 29;
    solver.expand(solver.board(), bottom_right, next_states, 0);
    valid = 0;
    for (const auto& s : next_states) {
        if (s.score != MIN_STATE_SCORE) {
            printf("initial %d, %d -> %d\n", s.begin, s.prev, s.curr);
            solver.print_board(s.board);
            assert(s.curr == 28 || s.curr == 23);
            valid++;
        }
    }
    assert(valid == 2);
    next_states.clear();
    next_states.resize(4);

    // three
    pazusoba::state three;
    three.curr = 3;
    three.prev = 3;
    three.begin = 3;
    solver.expand(solver.board(), three, next_states, 0);
    valid = 0;
    for (const auto& s : next_states) {
        if (s.score != MIN_STATE_SCORE) {
            printf("initial %d, %d -> %d\n", s.begin, s.prev, s.curr);
            solver.print_board(s.board);
            assert(s.curr == 2 || s.curr == 4 || s.curr == 9);
            valid++;
        }
    }
    assert(valid == 3);
    // 3 -> 2
    assert(solver.get_board_string(next_states[2].board) ==
           "DGRRBLHGBBGGRDDDDLBGHDBLLHDBLD");
    // 3 -> 4
    assert(solver.get_board_string(next_states[3].board) ==
           "DGRBRLHGBBGGRDDDDLBGHDBLLHDBLD");
    next_states.clear();
    next_states.resize(4);

    // location 1
    pazusoba::state loc_one;
    loc_one.curr = 1;
    loc_one.prev = 1;
    loc_one.begin = 1;
    solver.expand(solver.board(), loc_one, next_states, 0);
    valid = 0;
    for (const auto& s : next_states) {
        if (s.score != MIN_STATE_SCORE) {
            printf("initial %d, %d -> %d\n", s.begin, s.prev, s.curr);
            solver.print_board(s.board);
            assert(s.curr == 0 || s.curr == 2 || s.curr == 7);
            valid++;
        }
    }
    assert(valid == 3);
    next_states.clear();
    next_states.resize(4);

    // four
    pazusoba::state four;
    four.curr = 15;
    four.prev = 15;
    four.begin = 15;
    solver.expand(solver.board(), four, next_states, 0);
    valid = 0;
    for (const auto& s : next_states) {
        if (s.score != MIN_STATE_SCORE) {
            printf("initial %d, %d -> %d\n", s.begin, s.prev, s.curr);
            solver.print_board(s.board);
            assert(s.curr == 14 || s.curr == 16 || s.curr == 21 || s.curr == 9);
            valid++;
        }
    }
    assert(valid == 4);
    // 15 -> 9
    assert(solver.get_board_string(next_states[0].board) ==
           "DGRRBLHGBDGGRDDBDLBGHDBLLHDBLD");
    next_states.clear();

    printf("test expand passed\n");
    printf("====================================\n");
    printf("test explore - first step\n");
    next_states.resize(500);
    for (int i = 0; i < 30; i++) {
        pazusoba::state s;
        s.curr = i;
        s.prev = i;
        s.begin = i;
        solver.expand(solver.board(), s, next_states, i);
    }

    valid = 0;
    for (const auto& s : next_states) {
        if (s.score != MIN_STATE_SCORE) {
            printf("initial %d, %d -> %d\n", s.begin, s.prev, s.curr);
            solver.print_board(s.board);
            valid++;
        }
    }
    // 4 * 2, 4 corners
    // (3 + 3 + 4 + 4) * 3, 14 edges
    // (3 * 4) * 4, 12 centers
    assert(valid == 98);

    printf("passed\n");
    printf("====================================\n");

    return 0;
}
