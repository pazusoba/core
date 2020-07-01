#include "solver.h"
#include <iostream>

int main() {
    PadSolver solver = PadSolver();
    solver.printBoard();
    std::cout << solver.getMaxCombo() << " combo\n";

    return 0;
}