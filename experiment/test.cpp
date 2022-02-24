#include <cassert>
#include "pazusoba.h"

pazusoba::game_board convert(const char*);

int main(int argc, char* argv[]) {
    auto solver = pazusoba::solver();
    const char* arg1[] = {"", "DGRRBLHGBBGGRDDDDLBGHDBLLHDBLD"};
    solver.parse_args(2, (char**)arg1);

    auto move_count_board = convert("DGRRBLHGBBGGRDDDDLBGHDBLLHDBLD");
    pazusoba::combo_list combos;
    // erase
    solver.erase_orbs(move_count_board, combos);
    // move down
    solver.move_orbs_down(move_count_board);
    solver.print_board(move_count_board);
    solver.erase_orbs(move_count_board, combos);
    solver.move_orbs_down(move_count_board);
    solver.print_board(move_count_board);
    for (const auto& c : combos) {
        printf("%d %lu\n", c.info, c.loc.size());
    }
    printf("total combos: %lu\n", combos.size());
    assert(combos.size() == 2);

    return 0;
}

pazusoba::game_board convert(const char* input) {
    pazusoba::game_board board;
    for (int i = 0; i < strlen(input); i++) {
        char orb_char = input[i];
        // find the orb name from ORB_WEB_NAME and make it a number
        bool found = false;
        for (pazusoba::orb j = 0; j < ORB_COUNT; j++) {
            if (orb_char == pazusoba::ORB_WEB_NAME[j]) {
                found = true;
                board[i] = j;
                break;
            }
        }
    }
    return board;
}