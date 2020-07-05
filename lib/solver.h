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
    PadBoard board;

    // Read board from filePath, return the board
    Board readBoard(std::string filePath);

public:
    PadSolver(std::string filePath, int minEraseCondition);
    // Solve the current board
    std::string solve(int steps = 25);
};

#endif
