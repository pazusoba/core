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
#include "route.h"
#include "board.h"
#include "state.h"

class PSolver
{
    int row = 0;
    int column = 0;
    int minErase = 3;
    int steps = 25;
    int size = 1000;
    bool debug = true;

    /// Read board from filePath, return the board
    Board readBoard(const std::string &filePath);

public:
    /// This is the original board
    PBoard board;

    PSolver(const std::string &filePath, int minErase, int steps, int size);

    /// Solve the current board
    std::vector<Route> solve();

    /// Read a board from a string
    void setBoardFrom(const std::string &board);

    /// A random board that is row x column
    void setRandomBoard(int row, int column);

    /// Update beam size, mainly for Qt
    void setBeamSize(int size);

    /// Update step limit
    void setStepLimit(int step);
};

#endif
