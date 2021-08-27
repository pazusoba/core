#include <fmt/color.h>
#include <pazusoba/board.h>
#include <pazusoba/hash.h>

namespace pazusoba {

Board::Board(const board& board) {
    _board = board;
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

void Board::floodfill(pint x, pint y, const orb& orb, bool initial) {
    //
}

void Board::eraseOrbs(const std::function<void(const Combo&)>& f) {
    //
    for (int i = _row - 1; i >= 0; i--) {
        for (int j = 0; j < _column; j++) {
            auto orb = (*this)(i, j);
            // Ignore empty orbs
            if (orb == pad::empty)
                continue;

            floodfill(i, j, orb, true);
        }
    }
}

void Board::moveOrbsDown() {
    // TODO: maybe should taking min erase into account
    // because it is impossible to erase only one orb
    for (pint i = 0; i < _column; ++i) {
        int emptyIndex = -1;
        // signed type is needed or otherwise, j >= won't terminate at all
        // because after -1 is the max value again
        for (int j = _row - 1; j >= 0; --j) {
            auto index = INDEX_OF(j, i);
            auto orb = (*this)[index];
            if (orb == 0) {
                // Don't override empty index if available
                if (emptyIndex == -1)
                    emptyIndex = j;
            } else if (emptyIndex != -1) {
                // replace last known empty index
                // and replace it with current index
                (*this)(emptyIndex, i) = orb;
                (*this)[index] = 0;
                // simply move it up from last index
                --emptyIndex;
            }
        }
    }
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
                fmt::print("{}\t", pad::ORB_NAME[orb]);
                break;
            case PrintStyle::colourful:
                // bg -> background, fg -> foreground
                fmt::print(bg(pad::ORB_COLOURS[orb]), "  ");
                fmt::print("  ");
                break;
        }
    }
    fmt::print("\n============\n");
}

std::string Board::getFormattedBoard(FormatStyle style) const {
    std::string boardString("");
    for (pint i = 0; i < _size; i++) {
        auto orb = pint(_board[i]);
        switch (style) {
            case FormatStyle::dawnglare:
                boardString += pad::ORB_WEB_NAME[orb];
                break;
            case FormatStyle::serizawa:
                boardString += pad::ORB_WEB_MAKER_NAME[orb];
                break;
        }
    }
    return boardString;
}

orb& Board::operator[](pint index) {
    // TODO: when settings it for test, size can be 0
    // but can be removed safely after testing
    if (_size > 0 && index >= _size)
        return _empty;
    return _board[index];
}

orb& Board::operator()(pint x, pint y) {
    auto index = INDEX_OF(x, y);
    return (*this)[index];
}
}  // namespace pazusoba
