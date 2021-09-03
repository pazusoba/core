//
// state.h
// State contains the board and also extra information about the game.
// It contains some helper functions to spawn new states.
//
// Created by Yiheng Quan on 01/08/2021
//

#ifndef _STATE_H_
#define _STATE_H_

#include <functional>
#include "board.h"
#include "constant.h"

namespace pazusoba {
class State {
    Board _board;
    pint _currIndex = 0;
    // so that it doesn't go backwards
    pint _prevIndex = 0;

    pint _score = 0;
    pint _currentStep = 0;
    pint _maxStep = 0;

    // this decides if we cut off this state
    int _improvement = 0;
    int _countdown = 10;

    void calculateScore();

public:
    State() {}
    /// This is only used in the initial state
    State(const Board& board, pint maxStep, pint curr);
    State(const Board& board,
          pint currStep,
          pint maxStep,
          pint prev,
          pint curr,
          pint score,
          int countdown);

    std::deque<State> children(bool) const;
    size_t hash() const { return _board.hash(); }
    bool shouldCutOff() const { return _countdown <= 0; }

    const int& improvement() const { return _improvement; }
    const pint& score() const { return _score; }
    const Board& board() const { return _board; }
    const pint& currentStep() const { return _currentStep; }
    const pint& currIndex() const { return _currIndex; }
    const pint& prevIndex() const { return _prevIndex; }
    const pint& maxStep() const { return _maxStep; }

    bool operator<(const State&) const;
};
}  // namespace pazusoba

#endif
