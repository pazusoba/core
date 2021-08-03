//
// route.h
// Save route in an array, used in state.
//
// Created by Yiheng Quan on 01/08/2021
//

#ifndef _ROUTE_H_
#define _ROUTE_H_

#include <array>
#include "constant.h"

namespace pazusoba {

#define MAX_STEPS 100
typedef std::array<constant::Direction, MAX_STEPS> StepList;

// TODO: is it necessary to
class Route {
    StepList _steps;
    uint _totalSteps = 0;
    uint _startingLocation;

public:
    /// Pass in the starting location so that the correct route can be writen
    /// properly
    Route(uint loc) : _startingLocation(loc) {}

    void addNextStep(constant::Direction);
    void printRoute();
    /// Legacy support, python can read the step list directly
    void writeToDisk();
    /// Go back steps for rewind
    void goGack(uint);
    /// Shorten the route by removing unnessary moves
    void shorten();

    const StepList& steps() const { return _steps; }
    const uint& totalSteps() const { return _totalSteps; }

    constant::Direction& operator[](uint index);
};
}  // namespace pazusoba

#endif