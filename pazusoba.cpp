#include "core/v1/solver.h"

/// This is an interface for calling pazusoba
extern "C" void pazusoba(int argc, char *argv[]) {
    // std::string filePath = "assets/sample_board_floodfill_bug.txt";
    std::string filePath = "RHGHDRGLBLHGDBLLHBBBHRLHGHDGLB";
    // std::string filePath = "LHHLHDLDDLHLDDLDHDDDLLHDLHDHHDHHLDLDLHDHDH";
    int minErase = 3;
    int maxStep = 50;
    int maxSize = 10000;

    // Read from command line
    if (argc > 1) {
        filePath = argv[1];
    }
    if (argc > 2) {
        minErase = atoi(argv[2]);
        // min 3, max 5 for now
        if (minErase < 3)
            minErase = 3;
        if (minErase > 5)
            minErase = 5;
    }
    if (argc > 3) {
        maxStep = atoi(argv[3]);
    }
    if (argc > 4) {
        maxSize = atoi(argv[4]);
    }

    auto soba = new PSolver(filePath, minErase, maxStep, maxSize);
    soba->solve();
    delete soba;
}
