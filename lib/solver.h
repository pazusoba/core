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

class PadSolver
{
    PadBoard board;

public:
    PadSolver();
    PadSolver(std::string filePath, int minEraseCondition);
    void debug();
};

#endif
