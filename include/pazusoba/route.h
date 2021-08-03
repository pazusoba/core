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
    pint _totalSteps = 0;
    pint _startingLocation;

public:
    /// Pass in the starting location so that the correct route can be writen
    /// properly
    Route(pint loc) : _startingLocation(loc) {}

    void addNextStep(constant::Direction);
    void printRoute();
    /// Legacy support, python can read the step list directly
    void writeToDisk();
    /// Go back steps for rewind
    void goGack(pint);
    /// Shorten the route by removing unnessary moves
    void shorten();

    const StepList& steps() const { return _steps; }
    const pint& totalSteps() const { return _totalSteps; }

    constant::Direction& operator[](pint index);
};
}  // namespace pazusoba

#endif
