#include <fmt/core.h>
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
             pint curr,
             int score,
             int countdown) {
    _currentStep = currStep;
    _maxStep = maxStep;
    _board = board;
    _prevIndex = prev;
    _currIndex = curr;
    calculateScore();

    // track improvements
    // _improvement = (int)(_score - score);
    // if (_currentStep > maxStep / 4 && _improvement <= 0) {
    //     _countdown = countdown - 1;
    // }
}

void State::calculateScore() {
    ComboList list;

    pint combo = 0;
    int moveCount = 0;
    auto temp = _board;
    while (true) {
        temp.eraseOrbs(list);
        pint comboCount = list.size();
        // Check if there are more combo
        if (comboCount > combo) {
            temp.moveOrbsDown();
            combo = comboCount;
            moveCount++;
        } else {
            break;
        }
    }

    // Show the combo list and original board
    // _board.printBoard(colourful);
    // auto col = _board.column();
    // for (const auto& c : list) {
    //     fmt::print("orb: {}\n", pad::ORB_NAME[c.info]);
    //     for (const auto& loc : c.loc) {
    //         auto x = loc / col + 1;
    //         auto y = loc % col + 1;
    //         fmt::print("({}, {}) ", x, y);
    //     }
    //     fmt::print("\n");
    // }

    auto row = _board.row();
    auto column = _board.column();
    for (pint i = 0; i < row; i++) {
        for (pint j = 0; j < column; j++) {
            auto orb = _board(i, j);
            if (orb == _board(i + 1, j))
                _score += 2;
            else if (orb == _board(i - 1, j))
                _score += 2;
            else if (orb == _board(i, j + 1))
                _score += 2;
            else if (orb == _board(i, j - 1))
                _score += 2;
        }
    }

    _combo = list.size();
    _score += _combo * 50;
    _score -= moveCount * 50;
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

std::deque<State> State::children(bool diagonal) const {
    if (_currentStep == _maxStep)
        return {};

    std::deque<State> childrenState;
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
        childrenState.emplace_back(_newBoard, _currentStep + 1, _maxStep,
                                   _currIndex, newIndex, _score, _countdown);
    }

    return childrenState;
}

bool State::operator<(const State& a) const {
    return _score < a._score;
}
}  // namespace pazusoba
