//
// state.h
// State contains the board and also extra information about the game.
// It contains some helper functions to spawn new states.
//
// Created by Yiheng Quan on 01/08/2021
//

#ifndef _STATE_H_
#define _STATE_H_

#include "board.h"
#include "constant.h"

namespace pazusoba {
class State {
    Board _board;
    pint _currentIndex = 0;

    pint _score = 0;
    pint _currentStep = 0;
    pint _maxStep = 0;

    void calculateScore();

public:
    /// This is only used in the initial state
    State(const Board& board, pint maxStep, pint index);

    State next(pad::Direction);

    bool operator<(const State&) const;
};
}  // namespace pazusoba

#endif
