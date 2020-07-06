/**
 * state.cpp
 * by Yiheng Quan
 */

#include <iostream>
#include <algorithm>
#include "state.h"

std::map<int, State::StateTree> State::visitedState;

State::State(PadBoard *board, OrbLocation from, OrbLocation to, int step, int maxStep, int maxScore)
{
    this->board = board;
    this->previous = from;
    this->current = to;
    this->step = step;
    this->maxStep = maxStep;
    this->maxScore = maxScore;
}

bool State::isWorthy()
{
    // Stop immediately
    if ((step > maxStep / 3 && hasBeenVisited()) || step > maxStep)
        return false;

    int expected = 0;
    // if (step > 4)
    // {
    //     expected += step * 500;
    // }

    return score > expected;
}

bool State::solve()
{
    // Update the board before solving
    board->swapLocation(current, previous);
    PadBoard copy = *board;
    this->score = copy.rateBoard();

    if (!isWorthy())
        return false;

    // Add this if it should be visited
    visitedState[score].push_back(*this);
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            // This is just current orb
            if (i == 0 && j == 0)
                continue;
            auto next = LOCATION(current.first + i, current.second + j);
            // Must not go back to the parent or choose the same current and a valid location
            if (board->validLocation(next) && next != previous)
            {
                auto nextState = State(board, current, next, step + 1, maxStep, maxScore);
                nextState.parent = this;
                children.push_back(nextState);
            }
        }
    }

    // Sort by score
    std::sort(children.begin(), children.end(), [](State a, State b) {
        return a.score > b.score;
    });

    // Only get best ones as the step increases
    int multipler = (maxStep - step) * 100 / (maxStep * 100);
    if (multipler == 0)
        multipler = 1;
    int maxCount = 8 / multipler;
    for (auto newState : children)
    {
        if (maxCount == 0)
            break;
        newState.solve();
        // Ask all children to revert the board
        newState.revertBoard();
        maxCount--;
    }

    return true;
}

bool State::hasBeenVisited()
{
    auto current = visitedState[score];
    if (current.size() == 0)
        return false;
    for (auto s : current)
    {
        // Check if this has the same parent as before
        if (s.step < step && s.parent == parent)
            return true;
    }
    return false;
}

void State::printState()
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

void State::revertBoard()
{
    board->swapLocation(current, previous);
}
