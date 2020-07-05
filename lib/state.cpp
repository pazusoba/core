/**
 * state.cpp
 * by Yiheng Quan
 */

#include "state.h"

State::State(PadBoard* board, OrbLocation from, OrbLocation to, int step) {
    auto copy = *board;
    this -> score = copy.rateBoard();
    this -> parent = from;
    this -> current = to;
    this -> step = step;
}

bool State::isWorthy() {
    // TODO: now all states are fine but change this later
    return score > 0;
}

bool State::solve() {
    bool shouldSolve = isWorthy();
    if (!shouldSolve) return false;

    

    return true;
}