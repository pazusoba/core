//
// solver.h
// An interface of all solvers.
//
// Created by Yiheng Quan on 04/07/2021
//

#ifndef _SOLVER_H_
#define _SOLVER_H_

#include "../parser.h"
#include "../route.h"

namespace pazusoba {
class Solver {
protected:
    const Parser& parser;

public:
    Solver(const Parser& parser) : parser(parser) {}
    virtual ~Solver() = default;

    virtual Route solve() = 0;
};
}  // namespace pazusoba

#endif
