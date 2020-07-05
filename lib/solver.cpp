/**
 * solver.cpp
 * by Yiheng Quan
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include "solver.h"

/// Constrcutors

PadSolver::PadSolver(std::string filePath)
{
    readBoard(filePath);
}

PadSolver::PadSolver(std::string filePath, int minEraseCondition) : PadSolver(filePath)
{
    this->minEraseCondition = minEraseCondition;
}

PadSolver::~PadSolver()
{
    for (auto row : board)
    {
        // Clean all rows inside
        row.clear();
    }
    // Clean the board
    board.clear();
}

/// Board related

void PadSolver::solveBoard()
{
    // For now, start from 0, 0.
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            // Move around
        }
    }
    
    printBoard(&board);
    auto copy = board;
    int score = rateBoard(&copy);
    if (printMoreMessages)
        printBoard(&copy);
    std::cout << "Score was " << score << " pt\n";
}
