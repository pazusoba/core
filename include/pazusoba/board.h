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

enum boardPrintStyles {
    test = 0,   // print it out like sample_board_65.txt
    name,       // print it with orb names
    colourful,  // print it with orb colours
};

enum boardFormatStyles {
    dawnglare = 0,  // letters used in dawnglare
    serizawa,       // string used in puzzdra_theory_maker style
};

class board {
    pazuboard internalBoard;

public:
    int row = 0;
    int column = 0;
    int size = 0;

    board();
    void printBoard(boardPrintStyles style) const;
    std::string getFormattedBoard(boardFormatStyles style) const;

    // size_t is unsigned
    orb& operator[](size_t index);
};
}  // namespace pazusoba

#endif
