/**
 * solver.hpp
 * by Yiheng Quan
 */

#ifndef PAD_SOLVER_H
#define PAD_SOLVER_H

#include <string>
#include <vector>
#include <set>
#include "pad.hpp"
#include "board.hpp"
#include "state.hpp"

class PSolver
{
    int minEraseCondition = 3;
    int steps = 25;
    int size = 1000;

    // Read board from filePath, return the board
    Board readBoard(std::string filePath);

public:
    // Row and column are used to count board size
    int row = 0;
    int column = 0;
    // This is the original board
    PBoard board;

    PSolver() {}
    PSolver(int minEraseCondition, int maxStep, int maxSize);
    PSolver(std::string filePath, int minEraseCondition, int maxStep, int maxSize);
    // Solve the current board
    void solve();
};

#endif
