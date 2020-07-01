#include "lib/solver.h"
#include <iostream>

int main() {
    PadSolver solver = PadSolver("assets/sample_board.txt");
    solver.printBoard();
    std::cout << solver.getMaxCombo() << " combo\n";

    return 0;
}