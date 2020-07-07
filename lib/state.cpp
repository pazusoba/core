/**
 * state.cpp
 * by Yiheng Quan
 */

#include <iostream>
#include <algorithm>
#include "state.h"

/// Constrctor

State::State(PadBoard board, OrbLocation from, OrbLocation to, int step, int maxStep, int maxScore)
{
    // Update the board by swapping orbs
    this->board = board;
    board.swapLocation(current, previous);

    // Make a temp copy of board and calculate the score
    PadBoard copy = board;
    this->score = copy.rateBoard();

    // Copy other variables
    this->previous = from;
    this->current = to;
    this->step = step;
    this->maxStep = maxStep;
    this->maxScore = maxScore;
}

bool operator>(const State &a, const State &b)
{
    return a.score > b.score;
}

/// Functions

StateTree State::getChildren()
{
    StateTree children;

    // from -1 to 1, all 8 directions
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            auto next = LOCATION(current.first + i, current.second + j);
            // Ignore current and previous location so only 7 possible locations
            if (next == current || next == previous)
                continue;

            // It must be a valid location so not out of bound
            if (board.validLocation(next))
            {
                // TODO: consider diagonal moves, it should be punished for high risk
                auto nextState = State(board, current, next, step + 1, maxStep, maxScore);
                nextState.parent = this;

                children.push_back(nextState);
            }
        }
    }

    return children;
}

/// Utils

void State::printState()
{
    std::cout << score << " - " << step << std::endl;
    board.printBoardForSimulation();

    State *curr = this;
    while (curr != NULL)
    {
        auto loc = curr->current;
        std::cout << "(" << loc.first << ", " << loc.second << ") -> ";
        curr = curr->parent;
    }
    std::cout << "NULL\n\n";
}
