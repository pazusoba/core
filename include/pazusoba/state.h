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
    board _board;
    pint _currentIndex = 0;

    pint _score = 0;
    pint _currentStep = 0;
    pint _maxStep = 0;

public:
    ///
    State(pint maxStep) : _maxStep(maxStep) {}
};
}  // namespace pazusoba

#endif
