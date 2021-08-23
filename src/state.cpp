#include <pazusoba/state.h>

namespace pazusoba {
State::State(const Board& board, pint maxStep, pint curr, pint prev) {
    _maxStep = maxStep;
    _currentStep = 0;
    _board = board;
    _currIndex = curr;
    _prevIndex = prev;
}

State::State(const Board& board, pint prev, pint curr) {
    _currentStep++;
    if (stopped())
        return;

    _board = board;
    _prevIndex = prev;
    _currIndex = curr;
    calculateScore();
}

void State::calculateScore() {
    auto temp = _board;
    auto list = temp.eraseOrbs();
    while (list.size() > 0) {
        temp.moveOrbsDown();
        list = temp.eraseOrbs();
    }

    // probably call the profile here to calculate the score
    // can parse the reference down
    _score = 0;
}

State State::next(pad::Direction direction) {
    switch (direction) {
        case pad::up_left:
            _board[_currIndex];
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

bool State::stopped() const {
    return _currentStep > _maxStep;
}

bool State::operator<(const State& a) const {
    return _score < a._score;
}
};  // namespace pazusoba
