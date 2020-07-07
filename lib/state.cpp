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
    // don't use current and previous because they are not yet initialised
    this->board.swapLocation(from, to);
    this->boardID = this->board.getBoardID();
    auto copy = this->board;
    this->score = copy.rateBoard();

    // Make a temp copy of board and calculate the score
    if (this->score > maxScore)
    {
        this->board.printBoardForSimulation();
    }

    // Copy other variables
    this->previous = from;
    this->current = to;
    this->step = step;
    this->maxStep = maxStep;
    this->maxScore = maxScore;
    this->children.clear();
}

State::~State()
{
    for (auto c : children)
    {
        delete c;
    }
    children.clear();
}

bool State::operator<(const State &a) const
{
    return score < a.score;
}

/// Functions

State::StateTree State::getChildren()
{
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
                auto nextState = new State(board.copy(), current, next, step + 1, maxStep, maxScore);
                nextState->parent = this;

                // Cut down useless branches
                int minScore = 0;
                double percentage = (double)step / (double)maxStep;
                minScore = (int)(maxScore * percentage);

                if (score > minScore)
                    // Only append if it has enough score
                    children.push_back(nextState);
                else
                    delete nextState;
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
