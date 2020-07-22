/**
 * route.hpp
 * This saves routes from states and prints a readable path
 * 
 * by Yiheng Quan
 */

#include <vector>
#include <iostream>
#include "board.hpp"
#include "state.hpp"
#include "pad.hpp"

typedef std::vector<pad::direction> Directions;

class Route
{
    PBoard finalBoard;
    // A list of direction
    Directions directions;
    OrbLocation start;
    int score;
    int step;

    // Convert the end state to Directions
    void convertFromState(const PState *s)
    {
        if (s != nullptr)
            stateBack(s, s->parent);
    }

    // Go all the way back to the start and push to directions
    void stateBack(const PState *curr, const PState *parent)
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

    // Convert orbLocation to a direction
    pad::direction getDirection(const OrbLocation &curr, const OrbLocation &prev)
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

public:
    Route(const PState *state)
    {
        finalBoard = state->board;
        score = state->score;
        step = state->step;
        convertFromState(state);
    }

    // Just for QT to show the current board
    inline std::vector<int> getBoardOrbs()
    {
        return finalBoard.getBoardOrbs();
    }

    inline void printRoute()
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
};
