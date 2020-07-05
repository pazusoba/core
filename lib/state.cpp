/**
 * state.cpp
 * by Yiheng Quan
 */

#include <iostream>
#include "state.h"

State::State(PadBoard *board, OrbLocation from, OrbLocation to, int step, int maxStep)
{
    // Update the board the moment this state is being created
    board->swapLocation(from->location, to);
    PadBoard copy = *board;

    this->board = board;
    this->score = copy.rateBoard();
    this->parent = from;
    this->location = to;
    this->step = step;
    this->maxStep = maxStep;
}

bool State::isWorthy()
{
    // Stop immediately
    if (step > maxStep)
    {
        if (score > 15000)
        {
            board->printBoard();
            std::cout << score << " - " << step << std::endl;
            board->printBoardForSimulation();

            State *curr = this;
            while (curr != NULL)
            {
                auto loc = curr->location;
                std::cout << "(" << loc.first << ", " << loc.second << ") -> ";
                curr = curr->parent;
            }
            std::cout << "NULL\n";
        }
        return false;
    }

    int expected = 0;
    if (step >= 5)
    {
        expected = 1000;
        expected += (step - 5) * 2500;
    }
    // TODO: now all states are fine but change this later
    return score > expected;
}

bool State::solve()
{
    if (!isWorthy())
        return false;

    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            auto next = LOCATION(location.first + i, location.second + j);
            // Must not go back to the parent and a valid location
            if (next != parent->location && board->validLocation(next))
            {
                auto nextState = State(board, next, step + 1, maxStep);
                nextState.parent = this;
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
    board->swapLocation(location, parent->location);
}