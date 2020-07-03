#include <iostream>
#include "lib/solver.h"

int main(int argc, char* argv[]) {
    std::string filePath = "assets/sample_board_65_1.txt";
    // Read from command line
    if (argc > 1) {
        filePath = argv[1];
    }

    PadSolver solver(filePath);
    solver.printBoard();
    std::cout << solver.getMaxCombo() << " combo\n";
    solver.printBoardInfo();

    return 0;
}
