//
// route.h
// Save route in an array, used in state.
//
// Created by Yiheng Quan on 01/08/2021
//

#ifndef _ROUTE_H_
#define _ROUTE_H_

#include <array>
/// TODO: Remove cstdlib and ctime
#include <cstdlib>
#include <ctime>
#include "constant.h"

namespace pazusoba {

#define MAX_STEPS 100
typedef std::array<pad::Direction, MAX_STEPS> StepList;

/// TO BE CONSIDERED: this is passed to python
struct CRoute {
    int x = 0;
    int y = 0;
    // Indicate the size of this list
    int size = 0;
};

// TODO: is it necessary to
class Route {
    StepList _steps;
    pint _totalSteps = 0;
    pint _startingLocation;

public:
    /// Pass in the starting location so that
    /// the correct route can be writen properly
    Route(pint loc) : _startingLocation(loc) {}

    void addNextStep(pad::Direction);
    void printRoute();
    /// Legacy support, python can read the step list directly
    void writeToDisk();
    /// Go back steps for rewind
    void goGack(pint);
    /// Shorten the route by removing unnessary moves
    void shorten();
    /// TODO: not finalised
    inline CRoute* toCRouteList() {
        srand(time(nullptr));
        int count = rand() % 100;
        if (count == 0)
            count = 1;

        auto routes = new CRoute[count];
        for (int i = 0; i < count; i++) {
            routes[i].x = rand() % 10000;
            routes[i].y = rand() % 10000;
        }
        routes[0].size = count;
        return routes;
    }

    const StepList& steps() const { return _steps; }
    const pint& totalSteps() const { return _totalSteps; }

    pad::Direction& operator[](pint index);
};
}  // namespace pazusoba

#endif
