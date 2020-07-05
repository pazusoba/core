/**
 * solver.cpp
 * by Yiheng Quan
 */

#include "solver.h"

/// Constrcutors
PadSolver::PadSolver(std::string filePath, int minEraseCondition)
{
    board = PadBoard(filePath, minEraseCondition);
}

void PadSolver::debug() {
    board.debug();
}
