//
// beamsearch.h
// The beam search implemenation of pazusoba.
//
// Created by Yiheng Quan on 01/08/2021
//

#ifndef _BEAM_SEARCH_H_
#define _BEAM_SEARCH_H_

#include "solver.h"

namespace pazusoba {
class BeamSearch : public Solver {
public:
    BeamSearch(const Parser& parser) : Solver(parser){};
    Route solve();
};
}  // namespace pazusoba

#endif
