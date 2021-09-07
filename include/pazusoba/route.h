//
// route.h
// Save route in an array, used in state.
//
// Created by Yiheng Quan on 01/08/2021
//

#ifndef _ROUTE_H_
#define _ROUTE_H_

#include <vector>
/// TODO: Remove cstdlib and ctime
#include <cstdlib>
#include <ctime>
#include "constant.h"

namespace pazusoba {
typedef std::vector<pint> StepList;

struct CRoute {
    int x = 0;
    int y = 0;
    // The size of this list
    int size = 0;
};

class Route {
    std::vector<pint> _list;
    pint _totalSteps = 0;
    pint _column = 0;

public:
    Route() {}
    Route(pint column) : _column(column) {}

    void addNextStep(pint);
    void printRoute() const;
    /// Legacy support, python can read the step list directly
    void writeToDisk();
    /// Go back steps for rewind
    void goGack(pint);
    /// Shorten the route by removing unnessary moves
    void shorten();
    CRoute* exportedRoute();

    const StepList& list() const { return _list; }
    const pint& totalSteps() const { return _totalSteps; }

    pint& operator[](pint index);
};
}  // namespace pazusoba

#endif
