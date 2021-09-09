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
#include "hash.h"
#include "route.h"

namespace pazusoba {
class State {
    Board _board;
    Board _erased;

    pint _currIndex = 0;
    // so that it doesn't go backwards
    pint _prevIndex = 0;
    Route _route;

    // if negative is allowed, the initial score must be low
    int _score = -9999;
    pint _combo = 0;
    pint _currentStep = 0;
    pint _maxStep = 0;

    // this decides if we cut off this state
    int _improvement = 0;
    int _countdown = 10;

    void computeScore();

public:
    /// This is only used in the initial state
    State(const Board& board, pint maxStep, pint curr);
    State(const Board& board,
          const Route& route,
          pint currStep,
          pint maxStep,
          pint prev,
          pint curr,
          int score,
          int countdown,
          bool compute = true);

    std::deque<State> children(bool, bool compute = true);
    // Return a list of all possible states in certain steps
    std::deque<State> allChildren(pint, bool);
    size_t hash() const {
        return hash::djb2_hash_shift(_board.hash(), int(_prevIndex));
    }
    bool shouldCutOff() const { return _countdown <= 0; }

    const int& improvement() const { return _improvement; }
    const int& score() const { return _score; }
    const pint& combo() const { return _combo; }
    const Board& board() const { return _board; }
    const pint& currentStep() const { return _currentStep; }
    const pint& currIndex() const { return _currIndex; }
    const pint& prevIndex() const { return _prevIndex; }
    const pint& maxStep() const { return _maxStep; }
    Route& route() { return _route; }

    bool operator<(const State&) const;
};
}  // namespace pazusoba

#endif
