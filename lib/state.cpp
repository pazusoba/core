/**
 * state.cpp
 * by Yiheng Quan
 */

#include <iostream>
#include <cmath>
#include "state.h"

/// Constrctor

PState::PState(PBoard board, OrbLocation from, OrbLocation to, int step, int maxStep, int maxScore)
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

PState::~PState()
{
    for (auto c : children)
    {
        delete c;
    }
    children.clear();
}

bool PState::operator<(const PState &a) const
{
    return score < a.score;
}

bool PState::operator>(const PState &a) const
{
    // At least same step and more score
    return score / step > a.score / a.step;
}

/// Functions

PState::PStateList PState::getChildren()
{
    // Stop when max step has been reached or children has been collected before
    if (step > maxStep || children.size() > 0)
        return children;

    int totalScore = 0;
    int totalState = 0;
    // from -1 to 1, all 8 directions
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (
                (i == -1 && j == -1) ||
                (i == 1 && j == 1) ||
                (i == -1 && j == 1) ||
                (i == 1 && j == -1))
            {
                continue;
            }

            auto next = LOCATION(current.first + i, current.second + j);
            // Ignore current and previous location so only 7 possible locations
            if (next == current || next == previous)
                continue;

            // It must be a valid location so not out of bound
            if (board.validLocation(next))
            {
                // TODO: consider diagonal moves, it should be punished for high risk
                auto nextState = new PState(board.copy(), current, next, step + 1, maxStep, maxScore);
                nextState->parent = this;
                // Save the improvement
                nextState->improvement = nextState->score - score;
                // Track for average children score
                totalScore += nextState->score;
                totalState += 1;

                // auto nextChildren = nextState->getChildren();
                // bool isBetter = false;
                // for (auto c : nextChildren) {
                //     // See if next children is worse than current
                //     if (c->score > score) {
                //         isBetter = true;
                //         break;
                //     }
                // }

                // if (isBetter)
                //     children.push_back(nextState);
                // else
                //     delete nextState;

                children.push_back(nextState);
                
                // int minScore = 0;
                // // Cut down useless branches
                // // minScore = step / 3 * 800;

                // if (score > minScore)
                //     // Only append if it has enough score
                // else
                //     delete nextState;
            }
        }
    }

    averageScore = totalScore / totalState;
    return children;
}

/// Utils

void PState::printStateFromRoot(PState *parent)
{
    if (parent != NULL)
    {
        printStateFromRoot(parent->parent);
        auto loc = parent->current;
        int score = parent->score;
        std::cout << "(" << loc.first << ", " << loc.second << ") " << score << " -> ";
    }
}

void PState::printState()
{
    std::cout << score << " - " << step << std::endl;
    board.printBoardForSimulation();

    printStateFromRoot(this);
    std::cout << "NULL\n\n";
}
