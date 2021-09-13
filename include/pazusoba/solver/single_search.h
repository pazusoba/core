//
// singlesearch.h
// The beam search using a single thread
//
// Created by Yiheng Quan on 26/08/2021
//

#ifndef _SINGLE_SEARCH_H_
#define _SINGLE_SEARCH_H_

#include "solver.h"

namespace pazusoba {
class SingleSearch : public Solver {
public:
    SingleSearch(const Parser& parser) : Solver(parser) {}
    State solve();
};
}  // namespace pazusoba

#endif
