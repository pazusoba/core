#include <iostream>
#include "lib/solver.h"

/**
 * Return a solver pointer.
 * 
 * It handles user input from the console, mainly board file and min erase condition
 */
PSolver *handleInput(int, char **);

int main(int argc, char *argv[])
{
    auto soba = handleInput(argc, argv);
    soba->solve();
    delete soba;

    return 0;
}

PSolver *handleInput(int argc, char *argv[])
{
    std::string filePath = "assets/sample_board_65.txt";
    // std::string filePath = "RDHHLBBLHDHRBRDLHHDGRGBGLRGHLG";
    int minEraseCondition = 3;
    int maxStep = 25;
    int maxSize = 2000;

    // Read from command line
    if (argc > 1)
    {
        filePath = argv[1];
    }
    if (argc > 2)
    {
        minEraseCondition = atoi(argv[2]);
    }
    if (argc > 3)
    {
        maxStep = atoi(argv[3]);
    }
    if (argc > 4)
    {
        maxSize = atoi(argv[4]);
        // Cannot be more than 20000 or the threads might still crash
        if (maxSize > 50000)
            maxSize = 50000;
    }

    return new PSolver(filePath, minEraseCondition, maxStep, maxSize);
}
