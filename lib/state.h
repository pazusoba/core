/**
 * state.h
 * by Yiheng Quan
 */

#ifndef STATE_H
#define STATE_H

#include <map>
#include "board.h"

// A list of state
typedef std::vector<State> StateTree;

class State
{
    // The board
    PadBoard board;
    // This is the previous orb and it moved to current orb
    // Current must not move back to parent location
    OrbLocation previous;
    // This is current orb which is active (the player is holding it)
    OrbLocation current;
    // An estimation of a good score
    int maxScore;
    // This is max step we want to go (termination point)
    int maxStep;
    // ALl possible moves from this state
    StateTree children;

public:
    // This score shows how good the current board is
    int score = 0;
    // This is the number of steps (depth)
    int step;
    // Save the parent's address to track back
    State *parent = NULL;

    State(PadBoard board, OrbLocation from, OrbLocation to, int step, int maxStep, int maxScore);

    // This returns a list of all possible children states
    StateTree getChildren();

    // Print out current state, its board, score and more
    void printState();
};

#endif
