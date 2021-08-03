#include <fmt/color.h>
#include <pazusoba/board.h>
#include <pazusoba/hash.h>

namespace pazusoba {

Board::Board(const board& board) {
    _board = board;
}

bool Board::validateIndex(pint index) const {
    if (_size > 0 && index >= _size) {
        // if (error)
        //     throw std::out_of_range(fmt::format(
        //         "pazusoba::Board index {} out of range {}", index, _size -
        //         1));
        return false;
    } else if (index >= MAX_BOARD_SIZE) {
        // if (error)
        //     throw std::out_of_range(fmt::format(
        //         "pazusoba::Board index {} great than max board size {}",
        //         index, MAX_BOARD_SIZE));
        return false;
    }
    return true;
}

void Board::set(pint row, pint column) {
    _row = row;
    _column = column;
    _size = row * column;
}

void Board::swap(pint index1, pint index2) {
    const orb& one = (*this)[index1];
    if (one == 0)
        return;
    const orb& two = (*this)[index2];
    if (two == 0)
        return;

    auto temp = one;
    (*this)[index1] = two;
    (*this)[index2] = temp;
}

void Board::swap(pint one1, pint one2, pint two1, pint two2) {
    const orb& one = (*this)(one1, one2);
    if (one == 0)
        return;
    const orb& two = (*this)(two1, two2);
    if (two == 0)
        return;

    auto temp = one;
    (*this)(one1, one2) = two;
    (*this)(two1, two2) = temp;
}

Board Board::copy() const {
    auto newBoard = Board(_board);
    newBoard.set(_row, _column);
    return newBoard;
}

size_t Board::hash() const {
    unsigned char boardString[MAX_BOARD_SIZE] = {'\0'};
    for (pint i = 0; i < _size; i++) {
        boardString[i] = (unsigned char)(_board[i]);
    }
    return hash::djb2_hash(boardString);
}

void Board::printBoard(PrintStyle style) const {
    for (pint i = 0; i < _size; i++) {
        auto orb = pint(_board[i]);
        if (i > 0 && i % _column == 0) {
            fmt::print("\n");
            if (style == PrintStyle::colourful) {
                fmt::print("\n");
            }
        }

        switch (style) {
            case PrintStyle::test:
                fmt::print("{} ", orb);
                break;
            case PrintStyle::name:
                fmt::print("{}\t", constant::ORB_NAME[orb]);
                break;
            case PrintStyle::colourful:
                // bg -> background, fg -> foreground
                fmt::print(bg(constant::ORB_COLOURS[orb]), "  ");
                fmt::print("  ");
                break;
        }
    }
    fmt::print("\n============\n");
}

std::string Board::getFormattedBoard(FormatStyle style) const {
    std::string boardString = "";
    for (pint i = 0; i < _size; i++) {
        auto orb = pint(_board[i]);
        switch (style) {
            case FormatStyle::dawnglare:
                boardString += constant::ORB_WEB_NAME[orb];
                break;
            case FormatStyle::serizawa:
                boardString += constant::ORB_WEB_MAKER_NAME[orb];
                break;
        }
    }
    return boardString;
}

orb& Board::operator[](pint index) {
    if (validateIndex(index))
        return _board[index];
    return _empty;
}

orb& Board::operator()(pint x, pint y) {
    auto index = x * _column + y;
    if (validateIndex(index))
        return _board[index];
    return _empty;
}
}  // namespace pazusoba
