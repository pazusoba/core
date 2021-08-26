#include <pazusoba/state.h>

namespace pazusoba {
State::State(const Board& board, pint maxStep, pint curr) {
    _maxStep = maxStep;
    _currentStep = 0;
    _board = board;
    _currIndex = curr;
}

State::State(const Board& board,
             pint currStep,
             pint maxStep,
             pint prev,
             pint curr) {
    _currentStep = currStep;
    _maxStep = maxStep;
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

// Prevent code duplication
#define UP_DOWN_CHECK(index, size) \
    if (index >= size)             \
        continue;
#define LEFT_CHECK(index, column)          \
    if (index % column == 0 || index == 0) \
        continue;
#define RIGHT_CHECK(index, column) \
    if ((index + 1) % column == 0) \
        continue;

void State::children(const std::function<void(const State&)>& f,
                     bool diagonal) const {
    pint column = _board.column();
    pint size = _board.size();
    for (pint i = 0; i < pad::DIRECTION_COUNT; i++) {
        auto direction = pad::Direction(i);
        // ignore diagnoal moves, see constant.h for why the check is like this
        if (!diagonal && direction > pad::right)
            continue;

        pint newIndex = 0;
        switch (direction) {
            case pad::up:
                newIndex = _currIndex - column;
                UP_DOWN_CHECK(newIndex, size);
                break;
            case pad::down:
                newIndex = _currIndex + column;
                UP_DOWN_CHECK(newIndex, size);
                break;
            case pad::left:
                // 0, 6, 12, 18
                LEFT_CHECK(_currIndex, column);
                newIndex = _currIndex - 1;
                break;
            case pad::right:
                // 5, 11, 17, 23 -> 6, 12, 18, 24 ❌
                RIGHT_CHECK(_currIndex, column);
                newIndex = _currIndex + 1;
                break;
            case pad::up_left:
                LEFT_CHECK(_currIndex, column);
                newIndex = _currIndex - 1 - column;
                UP_DOWN_CHECK(newIndex, size);
                break;
            case pad::up_right:
                RIGHT_CHECK(_currIndex, column);
                newIndex = _currIndex + 1 - column;
                UP_DOWN_CHECK(newIndex, size);
                break;
            case pad::down_left:
                LEFT_CHECK(_currIndex, column);
                newIndex = _currIndex - 1 + column;
                UP_DOWN_CHECK(newIndex, size);
                break;
            case pad::down_right:
                RIGHT_CHECK(_currIndex, column);
                newIndex = _currIndex + 1 + column;
                UP_DOWN_CHECK(newIndex, size);
                break;
        }

        auto _newBoard = _board;
        _newBoard.swap(_currIndex, newIndex);
        f(State(_newBoard, _currentStep + 1, _maxStep, _currIndex, newIndex));
    }
}

bool State::stopped() const {
    return _currentStep > _maxStep;
}

bool State::operator<(const State& a) const {
    return _score < a._score;
}
};  // namespace pazusoba
