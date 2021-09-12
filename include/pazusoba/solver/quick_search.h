//
// quicksearch.h
// A quick implementation focusing on nothing but speed
//
// Created by Yiheng Quan on 26/08/2021
//

#ifndef _QUICK_SEARCH_H_
#define _QUICK_SEARCH_H_

#include "solver.h"

namespace pazusoba {
class QuickSearch : public Solver {
public:
    QuickSearch(const Parser& parser) : Solver(parser) {}
    State solve();
};
}  // namespace pazusoba

#endif
