//
// board.h
// A wrapper of the board array with helper functions
//
// Created by Yiheng Quan on 27/07/2021
//

#ifndef _BOARD_H_
#define _BOARD_H_

#include <array>
#include "constant.h"

namespace pazusoba {
typedef std::array<orb, MAX_BOARD_SIZE> pazuboard;

class board {
    // TODO: board should know row and column,
    // parser should only get it from board?
    int row;
    int column;
    pazuboard internalBoard;

public:
    board();

    orb& operator[](size_t index);
    const pazuboard& getBoard() const { return internalBoard; }
};
}  // namespace pazusoba

#endif
