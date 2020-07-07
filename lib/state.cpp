/**
 * state.cpp
 * by Yiheng Quan
 */

#include <iostream>
#include <cmath>
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
    if (children.size() > 0 && children.size() < 8)
    {
        for (auto c : children)
        {
            delete c;
        }
        children.clear();
    }
}

bool State::operator<(const State &a) const
{
    return score < a.score;
}

/// Functions

State::StateTree State::getChildren()
{
    // Stop when max step has been reached
    if (step > maxStep)
        return children;

    int totalScore = 0;
    int totalState = 0;
    // from -1 to 1, all 8 directions
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            int stepOffset = 1;
            // if (
            //     (i == -1 && j == -1) ||
            //     (i == 1 && j == 1) ||
            //     (i == -1 && j == 1) ||
            //     (i == 1 && j == -1))
            // {
            //     stepOffset = 2;
            // }

            auto next = LOCATION(current.first + i, current.second + j);
            // Ignore current and previous location so only 7 possible locations
            if (next == current || next == previous)
                continue;

            // It must be a valid location so not out of bound
            if (board.validLocation(next))
            {
                // TODO: consider diagonal moves, it should be punished for high risk
                auto nextState = new State(board.copy(), current, next, step + stepOffset, maxStep, maxScore);
                nextState->parent = this;

                totalScore += nextState->score;
                totalState += 1;

                int minScore = 0;
                // Cut down useless branches
                minScore = step / 3 * 800;

                if (score > minScore)
                    // Only append if it has enough score
                    children.push_back(nextState);
                else
                    delete nextState;
            }
        }
    }

    averageScore = totalScore / totalState;
    return children;
}

/// Utils

void State::printStateFromRoot(State *parent)
{
    if (parent != NULL)
    {
        printStateFromRoot(parent->parent);
        auto loc = parent->current;
        std::cout << "(" << loc.first << ", " << loc.second << ") -> ";
    }
}

void State::printState()
{
    std::cout << score << " - " << step << std::endl;
    board.printBoardForSimulation();

    printStateFromRoot(this);
    std::cout << "NULL\n\n";
}
