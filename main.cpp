#include <iostream>
#include "lib/solver.h"

/**
 * Return a solver pointer.
 * 
 * It handles user input from the console, mainly board file and min erase condition
 */
PadSolver *handleInput(int, char **);

int main(int argc, char *argv[])
{
    auto solver = handleInput(argc, argv);

    solver->printBoard();
    // Now, it only erases all combo
    solver->solveBoard();
    solver->printBoard();
    // solver->printBoardInfo();

    delete solver;
    return 0;
}

PadSolver *handleInput(int argc, char *argv[])
{
    std::string filePath = "assets/sample_board_65_2";
    int minEraseCondition = 3;

    // Read from command line
    if (argc > 1)
    {
        filePath = argv[1];
    }
    if (argc > 2)
    {
        minEraseCondition = atoi(argv[2]);
    }

    return new PadSolver(filePath, minEraseCondition);
}
