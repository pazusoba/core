#include <cassert>
#include "pazusoba.h"

int main(int argc, char* argv[]) {
    auto solver = pazusoba::solver();
    solver.set_board("DGRRBLHGBBGGRDDDDLBGHDBLLHDBLD");
    assert(solver.board_size() == 30);
    assert(solver.row() == 5);
    assert(solver.column() == 6);
    assert(solver.max_combo() == 8);
    assert(solver.min_erase() == 3);

    // pazusoba::combo_list combos;
    // // erase
    // solver.erase_orbs(move_count_board, combos);
    // // move down
    // solver.move_orbs_down(move_count_board);
    // solver.print_board(move_count_board);
    // solver.erase_orbs(move_count_board, combos);
    // solver.move_orbs_down(move_count_board);
    // solver.print_board(move_count_board);
    // for (const auto& c : combos) {
    //     printf("%d %lu\n", c.info, c.loc.size());
    // }
    // printf("total combos: %lu\n", combos.size());
    // assert(combos.size() == 2);

    return 0;
}
