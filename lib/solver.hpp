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
    // Row and column are used to count board size
    int row = 0;
    int column = 0;
    int minEraseCondition = 3;
    // This is the original board
    PBoard board;
    int steps = 20;
    int size = 1000;

    // Read board from filePath, return the board
    Board readBoard(std::string filePath);

public:
    PSolver() {}
    PSolver(int minEraseCondition, int maxStep, int maxSize);
    PSolver(std::string filePath, int minEraseCondition, int maxStep, int maxSize);
    // Solve the current board
    void solve();
};

#endif
