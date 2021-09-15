//
// quicksearch.h
// A quick implementation focusing on nothing but speed.
// A quick scan will be done so the algorithm can exit early.
//
// Created by Yiheng Quan on 26/08/2021
//

#ifndef _QUICK_SEARCH_H_
#define _QUICK_SEARCH_H_

#include "solver.h"

namespace pazusoba {
class QuickSearch : public Solver {
    struct greater {
        template <class T>
        bool operator()(const T& a, const T& b) const {
            return a > b;
        }
    };

    pint scan();

public:
    QuickSearch(const Parser& parser) : Solver(parser) {}
    State solve();
};
}  // namespace pazusoba

#endif
