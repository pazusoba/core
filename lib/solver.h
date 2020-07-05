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

/**
 * Shorten make_pair
 */
#define PAIR(x, y) (std::make_pair(x, y))

class PadSolver
{
public:
     PadSolver(std::string filePath);
     PadSolver(std::string filePath, int minEraseCondition);
     ~PadSolver();

     /**
         * Solve current board
         */
     void solveBoard();
};

#endif
