//
// solver.h
// An interface of all solvers.
//
// Created by Yiheng Quan on 04/07/2021
//

#ifndef _SOLVER_H_
#define _SOLVER_H_

#include "../parser.h"

namespace pazusoba {
class Solver {
public:
    Solver(const Parser& p);
    virtual ~Solver() = default;
    virtual void solve() = 0;
};
}  // namespace pazusoba

#endif
