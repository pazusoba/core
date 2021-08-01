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
    size_t _row = 0;
    size_t _column = 0;
    size_t _size = 0;

    void validateIndex(size_t);

public:
    Board() {}
    Board(const board& board);

    void printBoard(PrintStyle) const;
    std::string getFormattedBoard(FormatStyle) const;
    size_t hash() const;
    Board copy() const;

    const size_t& row() const { return _row; }
    const size_t& column() const { return _column; }
    const size_t& size() const { return _size; }

    void set(const size_t& row, const size_t& column) {
        _row = row;
        _column = column;
        _size = row * column;
    }

    // size_t is unsigned
    orb& operator[](size_t index);
    orb& operator()(size_t x, size_t y);
};
}  // namespace pazusoba

#endif
