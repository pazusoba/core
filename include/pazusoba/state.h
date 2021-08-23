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
// This should be off by default because it is risky most of the time
#define ALLOW_DIAGONAL_MOVES true

class State {
    Board _board;
    pint _currIndex = 0;
    // so that it doesn't go backwards
    pint _prevIndex = 0;

    pint _score = 0;
    pint _currentStep = 0;
    pint _maxStep = 0;

    void calculateScore();

public:
    /// This is only used in the initial state
    State(const Board& board, pint maxStep, pint curr, pint prev);
    State(const Board& board, pint prev, pint curr);

    void children(const std::function<void(const State&)>&) const;
    bool stopped() const;

    const pint& score() const { return _score; }
    const pint& currentStep() const { return _currentStep; }
    const pint& maxStep() const { return _maxStep; }

    bool operator<(const State&) const;
};
}  // namespace pazusoba

#endif
