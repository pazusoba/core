#include <fmt/core.h>
#include <pazusoba/state.h>

namespace pazusoba {
State::State(const Board& board, pint maxStep, pint curr) {
    _maxStep = maxStep;
    _currentStep = 0;
    _board = board;
    _erased = board;
    _currIndex = curr;
    _route = Route(_board.column());
    _route.addNextStep(_currIndex);
}

State::State(const Board& board,
             const Route& route,
             pint currStep,
             pint maxStep,
             pint prev,
             pint curr,
             int score,
             int countdown) {
    if (currStep > maxStep) {
        return;
    }

    _currentStep = currStep;
    _maxStep = maxStep;
    _board = board;
    _erased = board;
    _prevIndex = prev;
    _currIndex = curr;
    // Parent should pass the route down here so need to make a copy
    _route = route;

    calculateScore();

    // track improvements
    // _improvement = (int)(_score - score);
    // if (_currentStep > maxStep / 4 && _improvement <= 0) {
    //     _countdown = countdown - 1;
    // }
}

void State::calculateScore() {
    ComboList list;
    _score = 0;

    pint combo = 0;
    int moveCount = 0;
    while (true) {
        _erased.eraseOrbs(list);
        pint comboCount = list.size();
        // Check if there are more combo
        if (comboCount > combo) {
            _erased.moveOrbsDown();
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

    auto column = _board.column();
    struct OrbDist {
        pint max = 0;
        pint min = 0;
    };
    std::array<OrbDist, pad::ORB_COUNT> orbInfo;
    for (pint i = 0; i < _board.size(); i++) {
        auto orb = _board[i];
        auto y = i % column;
        auto info = orbInfo[orb];
        if (y > info.max)
            orbInfo[orb].max = y;
        else if (y < info.min)
            orbInfo[orb].min = y;
    }
    // move orbs closer Y
    for (const auto& info : orbInfo) {
        _score -= (info.max - info.min) * 4;
    }

    auto minErase = _board.minErase();
    for (const auto& c : list) {
        _score -= (c.loc.size() - minErase) * 10;
    }

    _combo = list.size();
    _score += _combo * 20;
    // _score += moveCount * 2;
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

std::deque<State> State::children(bool diagonal) {
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

        // Don't go back to the previous state
        if (newIndex == _prevIndex)
            continue;

        auto _newBoard = _board;
        _newBoard.swap(_currIndex, newIndex);
        auto _newRoute = _route;
        _newRoute.addNextStep(newIndex);
        childrenState.emplace_back(_newBoard, _newRoute, _currentStep + 1,
                                   _maxStep, _currIndex, newIndex, _score,
                                   _countdown);
    }

    return childrenState;
}

std::deque<State> State::allChildren(pint step, bool diagonal) {
    std::deque<State> childrenState;
    childrenState.push_front(*this);
    for (pint i = 0; i < step; i++) {
        std::deque<State> temp;
        while (!childrenState.empty()) {
            auto current = childrenState.front();
            childrenState.pop_front();
            for (const auto& state : current.children(diagonal)) {
                temp.push_back(state);
            }
        }
        childrenState = temp;
    }

    return childrenState;
}

bool State::operator<(const State& a) const {
    // <= is important here, < shouldn't be used
    if (_score == a._score) {
        return _currentStep <= a._currentStep;
    }
    return _score <= a._score;
}
}  // namespace pazusoba
