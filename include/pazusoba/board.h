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
#include <functional>
#include <vector>
#include "constant.h"

namespace pazusoba {

// 7x6
#define MAX_BOARD_SIZE 42
#define INDEX_OF(x, y) (x * _column + y);
// NOTE: if memory is not a concern, use int instead
typedef unsigned int orb;
typedef std::array<orb, MAX_BOARD_SIZE> board;

/// Since Orb is always the same, we can simply store locations here
struct Combo {
    orb info;
    std::vector<pint> loc;
    Combo(const orb& o) : info(o) {}
};
typedef std::vector<Combo> ComboList;

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
    /// Used for empty reference
    orb _empty = 0;

public:
    Board() {}
    Board(const board& board);

    /// Set row, column and size of the board
    void set(pint row, pint column);
    void swap(pint, pint);
    void swap(pint, pint, pint, pint);
    void eraseOrbs(const std::function<void(Combo)>&);
    void moveOrbsDown();
    Board copy() const;
    size_t hash() const;

    void printBoard(PrintStyle) const;
    std::string getFormattedBoard(FormatStyle) const;

    const pint& row() const { return _row; }
    const pint& column() const { return _column; }
    const pint& size() const { return _size; }

    /// 0 is returned to indicate that the index is out of bound.
    /// This is useful when swapping two orbs
    orb& operator[](pint index);
    orb& operator()(pint x, pint y);
};
}  // namespace pazusoba

#endif
