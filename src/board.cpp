#include <fmt/color.h>
#include <pazusoba/board.h>
#include <pazusoba/hash.h>

namespace pazusoba {

Board::Board(const board& board) {
    _board = board;
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

void Board::validateIndex(pint index) {
    if (_size > 0 && index >= _size) {
        // If the board is 6x5, the index should never go over 29,
        // this can be treated as a test as well.
        throw std::out_of_range(fmt::format(
            "pazusoba::Board index {} out of range {}", index, _size - 1));
    } else if (index >= MAX_BOARD_SIZE) {
        throw std::out_of_range(
            fmt::format("pazusoba::Board index {} great than max board size {}",
                        index, MAX_BOARD_SIZE));
    }
}

orb& Board::operator[](pint index) {
    validateIndex(index);
    return _board[index];
}

orb& Board::operator()(pint x, pint y) {
    auto index = x * _column + y;
    validateIndex(index);
    return _board[index];
}
}  // namespace pazusoba
