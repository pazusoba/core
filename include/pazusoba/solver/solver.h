//
// solver.h
// An interface of all solvers.
//
// Created by Yiheng Quan on 04/07/2021
//

#ifndef _SOLVER_H_
#define _SOLVER_H_

#include "../parser.h"
#include "../state.h"

namespace pazusoba {
class Solver {
protected:
    const Parser& _parser;

public:
    Solver(const Parser& parser) : _parser(parser) {}
    virtual ~Solver() = default;

    /// Return a single route
    virtual State solve() = 0;
};
}  // namespace pazusoba

#endif
