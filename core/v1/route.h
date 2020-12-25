/**
 * route.h
 * This saves routes from states and prints a readable path
 * 
 * by Yiheng Quan
 */

#ifndef ROUTE_H
#define ROUTE_H

#include <vector>
#include <iostream>
#include "board.h"
#include "state.h"
#include "pad.h"

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
    void convertFromState(const PState *s);

    // Go all the way back to the start and push to directions
    void stateBack(const PState *curr, const PState *parent);

    // Convert orbLocation to a direction
    pad::direction getDirection(const OrbLocation &curr, const OrbLocation &prev);

public:
    Route(const PState *state);

    void printRoute();
};

#endif
