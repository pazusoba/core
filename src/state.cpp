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
};  // namespace pazusoba
