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
    ComboList list;
    auto func = [&list](const Combo& c) { list.push_back(c); };

    temp.eraseOrbs(func);
    while (list.size() > 0) {
        temp.moveOrbsDown();
        temp.eraseOrbs(func);
    }

    // probably call the profile here to calculate the score
    // can parse the reference down
    _score = 0;
}

void State::children(const std::function<void(const State&)>& func) const {
    // getting all possible children
}

bool State::stopped() const {
    return _currentStep > _maxStep;
}

bool State::operator<(const State& a) const {
    return _score < a._score;
}
};  // namespace pazusoba
