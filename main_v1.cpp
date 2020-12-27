#include <iostream>
#include "core/v1/solver.h"

/// This handles user input from the console, mainly board file and min erase condition
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
    // std::string filePath = "assets/sample_board_4erase.txt";
    std::string filePath = "RHGHDRGLBLHGDBLLHBBBHRLHGHDGLB";
    // this one had some problems
    // std::string filePath = "LHHLHDLDDLHLDDLDHDDDLLHDLHDHHDHHLDLDLHDHDH";
    int minErase = 3;
    int maxStep = 50;
    int maxSize = 500;

    // Read from command line
    if (argc > 1)
    {
        filePath = argv[1];
    }
    if (argc > 2)
    {
        minErase = atoi(argv[2]);
        // min 3, max 5 for now
        if (minErase < 3)
            minErase = 3;
        if (minErase > 5)
            minErase = 5;
    }
    if (argc > 3)
    {
        maxStep = atoi(argv[3]);
    }
    if (argc > 4)
    {
        maxSize = atoi(argv[4]);
    }

    return new PSolver(filePath, minErase, maxStep, maxSize);
}
