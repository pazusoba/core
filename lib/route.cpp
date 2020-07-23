/**
 * route.cpp
 * by Yiheng Quan
 */

#include "route.h"

Route::Route(const PState *state)
{
    finalBoard = state->board;
    score = state->score;
    step = state->step;
    convertFromState(state);
}

void Route::printRoute()
{
    using namespace std;
    cout << score << " - " << step << "\n";
    finalBoard.printBoardForSimulation();
    cout << "(" << start.first + 1 << ", " << start.second + 1 << ") ";
    for (const auto &d : directions)
    {
        cout << pad::DIRECTION_NAMES[d] << " ";
    }
    cout << "\n\n";
}

void Route::convertFromState(const PState *s)
{
    if (s != nullptr)
        stateBack(s, s->parent);
}

void Route::stateBack(const PState *curr, const PState *parent)
{
    if (parent != nullptr)
    {
        stateBack(parent, parent->parent);
        directions.push_back(getDirection(curr->current, curr->previous));
    }
    else
    {
        // When parent is null, we have reached step 0
        start = curr->current;
    }
}

pad::direction Route::getDirection(const OrbLocation &curr, const OrbLocation &prev)
{
    if (curr.first > prev.first)
    {
        // Moved down
        if (curr.second == prev.second)
            return pad::down;
        else if (curr.second > prev.second)
            return pad::downRight;
        else
            return pad::downLeft;
    }
    else if (curr.first < prev.first)
    {
        // Moved up
        if (curr.second == prev.second)
            return pad::up;
        else if (curr.second > prev.second)
            return pad::upRight;
        else
            return pad::upLeft;
    }
    else
    {
        // Moved left or right
        if (curr.second > prev.second)
            return pad::right;
        else
            return pad::left;
    }
}
