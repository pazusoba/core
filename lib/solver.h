/**
 * solver.h
 * by Yiheng Quan
 */

#ifndef PAD_SOLVER_H
#define PAD_SOLVER_H

#include <string>
#include <vector>
#include <set>
#include "pad.h"
#include "board.h"
#include "state.h"

class PadSolver
{
    // Row and column are used to count board size
    int row = 0;
    int column = 0;
    // This is the original board
    PadBoard board;
    int steps = 25;
    int size = 100;

    // Read board from filePath, return the board
    Board readBoard(std::string filePath);

public:
    PadSolver(std::string filePath, int minEraseCondition, int maxStep, int maxSize);
    // Solve the current board
    std::string solve();
};

#endif
