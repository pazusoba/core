/**
 * state.h
 * by Yiheng Quan
 */

#ifndef STATE_H
#define STATE_H

#include <map>
#include "board.h"

class State
{
    // A list of state
    typedef std::vector<State*> StateTree;

    // The board
    PadBoard board;
    // An estimation of a good score
    int maxScore;
    // This is max step we want to go (termination point)
    int maxStep;
    // ALl possible moves from this state
    StateTree children;

    // This is a recursive function to print from the beginning
    void printStateFromRoot(State *parent);

public:
    // This score shows how good the current board is
    int score = 0;
    // This can reflex how good this path is
    int averageScore = 0;
    // This is the number of steps (depth)
    int step;
    // This is the previous orb and it moved to current orb
    // Current must not move back to parent location
    OrbLocation previous;
    // This is current orb which is active (the player is holding it)
    OrbLocation current;
    // Save the parent's address to track back
    State *parent = NULL;
    // Every board has an unique id string
    std::string boardID;

    // This is only for testing
    State(int s) : score(s) {}
    State(PadBoard board, OrbLocation from, OrbLocation to, int step, int maxStep, int maxScore);
    ~State();

    // Override operators
    bool operator<(const State &a) const;
    bool operator>(const State &a) const;

    // This returns a list of all possible children states
    StateTree getChildren();

    // Print out current state, its board, score and more
    void printState();
};

#endif
