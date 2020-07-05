/**
 * state.cpp
 * by Yiheng Quan
 */

#include <iostream>
#include "state.h"

State::State(PadBoard *board, OrbLocation from, OrbLocation to, int step, int maxStep, int maxScore)
{
    // Update the board the moment this state is being created
    board->swapLocation(from, to);
    PadBoard copy = *board;

    this->board = board;
    this->score = copy.rateBoard();
    this->previous = from;
    this->current = to;
    this->step = step;
    this->maxStep = maxStep;
    this->maxScore = maxScore;
}

bool State::isWorthy()
{
    // Stop immediately
    if (step > maxStep)
        return false;

    int expected = 0;
    if (step > 5)
    {
        expected = step * 500;
    }

    if (score > maxScore)
    {
        std::cout << score << " - " << step << std::endl;
        board->printBoardForSimulation();

        State *curr = this;
        while (curr != NULL)
        {
            auto loc = curr->current;
            std::cout << "(" << loc.first << ", " << loc.second << ") -> ";
            curr = curr->parent;
        }
        std::cout << "NULL\n\n";
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
            auto next = LOCATION(current.first + i, current.second + j);
            // Must not go back to the parent or choose the same current and a valid location
            if (next != previous && next != current && board->validLocation(next))
            {
                auto nextState = State(board, current, next, step + 1, maxStep, maxScore);
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
    // revertBoard();

    return true;
}

void State::revertBoard()
{
    board->swapLocation(current, previous);
}