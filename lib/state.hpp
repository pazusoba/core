/**
 * state.hpp
 * by Yiheng Quan
 */

#ifndef STATE_H
#define STATE_H

#include <map>
#include "board.hpp"

class PState
{
    // A list of state
    typedef std::vector<PState *> PStateList;

    // The board
    PBoard board;
    // An estimation of a good score
    int maxScore;
    // This is max step we want to go (termination point)
    int maxStep;
    // ALl possible moves from this state
    PStateList children;

    // This is a recursive function to print from the beginning
    void printStateFromRoot(PState *parent);

public:
    // This score shows how good the current board is
    int score = 0;
    // This is the number of steps (depth)
    int step;
    // This is the previous orb and it moved to current orb
    // Current must not move back to parent location
    OrbLocation previous;
    // This is current orb which is active (the player is holding it)
    OrbLocation current;
    // Save the parent's address to track back
    PState *parent = NULL;
    // Every board has an unique id string
    std::string boardID;

    // This is only for testing
    PState(int s) : score(s) {}
    PState(const PBoard &board, const OrbLocation &from, const OrbLocation &to, int step, int maxStep, int maxScore);
    ~PState();

    // Override operators
    bool operator<(const PState &a) const;
    bool operator>(const PState &a) const;

    // This returns a list of all possible children states
    PStateList getChildren();

    // Print out current state, its board, score and more
    void printState();
};

#endif
