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
class board {
    std::array<orb, MAX_BOARD_SIZE> internalBoard;

public:
    board();

    orb& operator[](size_t index);
};
}  // namespace pazusoba

#endif
