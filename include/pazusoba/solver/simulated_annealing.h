//
// simulated_annealing.h
// The implementation of simulated annealing
// This accepts worse route with probability
//
// Created by Yiheng Quan on 14/02/2022
//

#ifndef _QUICK_SEARCH_H_
#define _QUICK_SEARCH_H_

#include "solver.h"

namespace pazusoba {
class SimulatedAnnealing : public Solver {
    int temperature = 100;

public:
    SimulatedAnnealing(const Parser& parser) : Solver(parser) {}
    State solve();
};
}  // namespace pazusoba

#endif
