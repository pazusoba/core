//
// board.h
// A wrapper of the board array with helper functions.
// Any calculation or changes are done in solver.
//
// Created by Yiheng Quan on 27/07/2021
//

#ifndef _BOARD_H_
#define _BOARD_H_

#include <array>
#include "constant.h"

namespace pazusoba {

// 7x6
#define MAX_BOARD_SIZE 42
// NOTE: if memory is not a concern, use int instead
typedef unsigned int orb;
typedef std::array<orb, MAX_BOARD_SIZE> pazuboard;

enum PrintStyle {
    test = 0,   // print it out like sample_board_65.txt
    name,       // print it with orb names
    colourful,  // print it with orb colours
};

enum FormatStyle {
    dawnglare = 0,  // letters used in dawnglare
    serizawa,       // string used in puzzdra_theory_maker style
};

class board {
    pazuboard internalBoard;

    void validateIndex(size_t);

public:
    int row = 0;
    int column = 0;
    int size = 0;

    board() {}

    void printBoard(PrintStyle) const;
    std::string getFormattedBoard(FormatStyle) const;
    size_t hash();

    // size_t is unsigned
    orb& operator[](size_t);
    orb& operator()(size_t, size_t);
};
}  // namespace pazusoba

#endif
