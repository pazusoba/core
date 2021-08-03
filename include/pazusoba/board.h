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
typedef std::array<orb, MAX_BOARD_SIZE> board;

enum PrintStyle {
    test = 0,   // print it out like sample_board_65.txt
    name,       // print it with orb names
    colourful,  // print it with orb colours
};

enum FormatStyle {
    dawnglare = 0,  // letters used in dawnglare
    serizawa,       // string used in puzzdra_theory_maker style
};

class Board {
    board _board;
    pint _row = 0;
    pint _column = 0;
    pint _size = 0;

    void validateIndex(pint);

public:
    Board() {}
    Board(const board& board);

    void printBoard(PrintStyle) const;
    std::string getFormattedBoard(FormatStyle) const;
    size_t hash() const;
    Board copy() const;

    const pint& row() const { return _row; }
    const pint& column() const { return _column; }

    /// Set row, column and size of the board
    inline void set(pint row, pint column) {
        _row = row;
        _column = column;
        _size = row * column;
    }

    // pint is unsigned
    orb& operator[](pint index);
    orb& operator()(pint x, pint y);
};
}  // namespace pazusoba

#endif
