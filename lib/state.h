/**
 * state.h
 * by Yiheng Quan
 */

#ifndef STATE_H
#define STATE_H

#include "board.h"

/**
 * State should have a board
 * Current orb location
 * Path?
 * Should I build a tree as I go?
 */

class State
{
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

public:
    State(PadBoard* board, OrbLocation from, OrbLocation to, int step);
};

#endif
