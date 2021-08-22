#include <pazusoba/state.h>

namespace pazusoba {
State::State(const Board& board, pint maxStep, pint index) {
    _maxStep = maxStep;
    _currentStep = 0;
    _board = board;
    _currentIndex = index;
}

void State::calculateScore() {
    _score = 0;
}

const State& State::next(pad::Direction direction) {
    switch (direction) {
        case pad::up_left:
            break;
        case pad::up:
            break;
        case pad::up_right:
            break;
        case pad::left:
            break;
        case pad::right:
            break;
        case pad::down_left:
            break;
        case pad::down:
            break;
        case pad::down_right:
            break;
    }
}

bool State::operator<(const State& a) const {
    return _score < a._score;
}
};  // namespace pazusoba
