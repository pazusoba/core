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
    int minEraseCondition = 3;
    int steps = 25;
    int size = 1000;
    bool DEBUG = true;

    // Read board from filePath, return the board
    Board readBoard(std::string &filePath);

public:
    // Row and column are used to count board size
    int row = 0;
    int column = 0;
    // This is the original board
    PBoard board;

    PSolver() {}
    PSolver(int minEraseCondition, int maxStep, int maxSize);
    PSolver(std::string &filePath, int minEraseCondition, int maxStep, int maxSize);

    // Solve the current board
    std::vector<Route> solve();

    // Read a board from a string
    void setBoardFrom(std::string &board);

    // A random board that is row x column
    void setRandomBoard(int row, int column);

    // Update beam size, mainly for Qt
    void setBeamSize(int size);

    // Update step limit
    void setStepLimit(int step);
};

#endif
