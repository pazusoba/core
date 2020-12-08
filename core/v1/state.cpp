/**
 * state.cpp
 * by Yiheng Quan
 */

#include <iostream>
#include <cmath>
#include "state.h"

// MARK: - Constrctor

PState::PState(const PBoard &board, const OrbLocation &from, const OrbLocation &to, int step, int maxStep)
{
    // Update the board by swapping orbs
    this->board = board;
    // don't use current and previous because they are not yet initialised
    this->board.swapLocation(from, to);
    auto copy = this->board;
    this->score = copy.rateBoard();

    // Copy other variables
    this->previous = from;
    this->current = to;
    this->step = step;
    this->maxStep = maxStep;
    // Max 7 positions
    this->children.reserve(7);
}

PState::~PState()
{
    if (children.size() > 0)
    {
        for (auto const &c : children)
        {
            mtx.lock();
            delete c;
            mtx.unlock();
        }
        children.clear();
    }
}

bool PState::operator<(const PState &a) const
{
    // 0 - 99
    // int num = rand() % 100;
    // if (num < 50)
    //     return true;
    return score < a.score;
}

bool PState::operator==(const PState &a) const
{
    // Same previous location
    if (step < a.step && previous == a.previous)
    {
        return board.hasSameBoard(&a.board);
    }

    return false;
}

// MARK: - Functions

PState::PStateList PState::getChildren()
{
    // Stop when max step has been reached or children has been collected before
    if (step > maxStep)
        return children;

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
                // Only allow diagonal when it is close to the end
                //                double percent = double(step) / double(maxStep);
                //                if (percent < 0.9)
                //                    continue;
                continue;
            }

            auto next = LOCATION(current.first + i, current.second + j);
            // Ignore current and previous location so only 7 possible locations
            if (next == current || next == previous)
                continue;

            // It must be a valid location so not out of bound
            if (board.validLocation(next))
            {
                // Setup new state and add this to children
                auto nextState = new PState(board, current, next, step + 1, maxStep);
                if (nextState != nullptr)
                {
                    nextState->parent = this;
                    mtx.lock();
                    children.push_back(nextState);
                    mtx.unlock();
                }
            }
        }
    }

    return children;
}

// MARK: - Utils

void PState::saveStateFromRoot(PState *parent, std::ofstream *file)
{
    if (parent != nullptr)
    {
        saveStateFromRoot(parent->parent, file);
        auto loc = parent->current;
        *file << loc.first << "," << loc.second << "|";
    }
}

void PState::saveToDisk()
{
    std::ofstream f;
    f.open("path.pazusoba");
    saveStateFromRoot(this, &f);
    f.close();
}

void PState::printStateFromRoot(PState *parent)
{
    if (parent != nullptr)
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
    std::cout << "nullptr\n\n";
}
