//
// board.h
// A wrapper of the board array with helper functions
//
// Created by Yiheng Quan on 27/07/2021
//

#ifndef _BOARD_H_
#define _BOARD_H_

#include <array>

// 7x6
#define MAX_BOARD_SIZE 42
// NOTE: if memory is not a concern, use int instead
typedef char Orb;
typedef std::array<Orb, MAX_BOARD_SIZE> Board;

namespace pazusoba {
class board {
    Board board{-1};

public:
};
}  // namespace pazusoba

#endif
