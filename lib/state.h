/**
 * state.h
 * by Yiheng Quan
 */

#ifndef STATE_H
#define STATE_H

#include "board.h"

class State
{
    // A list of state
    typedef std::vector<State> StateTree;

    PadBoard* board;
    // This is the previous orb and it moved to current orb
    // Current must not move back to parent location
    OrbLocation parent;
    // This is current orb which is active (the player is holding it)
    OrbLocation current;
    // This score shows how good the current board is
    int score;
    // This is the number of steps (depth)
    int step;
    // ALl possible moves from this state
    StateTree children;

    /**
     * There are too many possibilities so I need to cut down many of them.
     * Most moves are complicated useless and good moves will stand up with a good score.
     */
    bool isWorthy();

public:
    State(PadBoard* board, OrbLocation from, OrbLocation to, int step);

    // Keep solving until max steps have been reached
    // return - true if it is worthy
    bool solve();
};

#endif
