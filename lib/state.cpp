/**
 * state.cpp
 * by Yiheng Quan
 */

#include "state.h"

State::State(PadBoard *board, OrbLocation from, OrbLocation to, int step, int maxStep)
{
    // Don't waste time doing anything here
    if (step > maxStep)
        return;

    // Update the board the moment this state is being created   
    board->swapLocation(from, to);
    auto copy = *board;

    this->board = board;
    this->score = copy.rateBoard();
    this->parent = from;
    this->current = to;
    this->step = step;
}

bool State::isWorthy()
{
    // Stop immediately
    if (step > maxStep)
        return false;

    // TODO: now all states are fine but change this later
    return score > 0;
}

bool State::solve()
{
    if (!isWorthy())
        return false;

    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; i <= 1; j++)
        {
            auto next = LOCATION(current.first + i, current.second + j);
            // Must not go back to the parent and a valid location
            if (next != parent && board->validLocation(next))
            {
                auto nextState = State(board, current, next, step + 1, maxStep);
                // TODO: sort children by their score and go down further
                if (nextState.solve())
                {
                    children.push_back(nextState);
                }
                // Ask all children to revert the board
                nextState.revertBoard();
            }
        }
    }

    // After all children have reverted the board, current state can now revert the board
    revertBoard();

    return true;
}

void State::revertBoard()
{
    board->swapLocation(current, parent);
}