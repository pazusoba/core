#include <fmt/color.h>
#include <pazusoba/board.h>
#include <pazusoba/hash.h>

namespace pazusoba {
Board::Board(const board& board) {
    _board = board;
}

void Board::set(pint row, pint column, pint minErase) {
    _row = row;
    _column = column;
    _minErase = minErase;
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

void Board::floodfill(Combo& combo,
                      pint x,
                      pint y,
                      const orb& orb,
                      bool initial) {
    auto index = INDEX_OF(x, y);
    auto currOrb = (*this)[index];
    if (currOrb == 0)
        return;
    if (currOrb != orb && _visited[index] < 1)
        return;

    // track num of connected orbs, also include the current orb so start from 1
    int count = 1;
    // track all directions
    int dList[4] = {0, 0, 0, 0};
    // the min number of connected orbs to consider it as a combo
    int minConnection = initial ? _minErase : (_minErase >= 3 ? 3 : _minErase);

    // all 4 directions
    // 0 -> right
    // 1 -> left
    // 2 -> down
    // 3 -> up
    for (int d = 0; d < 4; d++) {
        int loop = _row;
        if (d > 1)
            loop = _column;

        // start from 1 to look for orbs around
        for (int i = 1; i < loop; i++) {
            pint cx = x;
            pint cy = y;
            if (d == 0)
                cy += i;
            else if (d == 1)
                cy -= i;
            else if (d == 2)
                cx += i;
            else if (d == 3)
                cx -= i;

            // Need to make sure both indexes are right
            if (cx < 0 || cy < 0 || cx >= _row || cy >= _column)
                break;

            auto cindex = INDEX_OF(cx, cy);
            // Make sure this is valid
            if (cindex >= _size)
                break;
            // stop if it doesn't match and it is not visited yet
            // if visited, it means this orb has the same colour
            if (_board[cindex] != orb && _visited[cindex] < 1)
                break;

            dList[d]++;
            count++;
        }
    }

    // more than erase condition
    if (count >= minConnection) {
        int start = 0;
        int end = 2;

        int hCount = dList[0] + dList[1] + 1;
        int vCount = dList[2] + dList[3] + 1;
        bool horizontal = hCount >= minConnection;
        bool vertical = vCount >= minConnection;
        // this is either L or +
        if (hCount == 3 && vCount == 3) {
            horizontal = true;
            vertical = true;
        }

        if (!horizontal)
            start = 1;
        if (!vertical)
            end = 1;

        // erase and do flood fill
        for (int d = start; d < end; d++) {
            int startCount = -(dList[d * 2 + 1]);
            int endCount = dList[d * 2] + 1;

            for (int i = startCount; i < endCount; i++) {
                int cx = x;
                int cy = y;
                if (d == 0)
                    cy += i;
                else if (d == 1)
                    cx += i;

                int index = INDEX_OF(cx, cy);
                if (_visited[index] == 0) {
                    _board[index] = 0;
                    combo.loc.emplace_back(index);
                    _visited[index] = 1;
                } else {
                    // shouldn't visit this orb again
                    _visited[index] = 2;
                }
            }
        }

        for (int d = start; d < end; d++) {
            int startCount = -(dList[d * 2 + 1]);
            int endCount = dList[d * 2] + 1;
            for (int i = startCount; i < endCount; i++) {
                int cx = x;
                int cy = y;
                if (d == 0)
                    cy += i;
                else if (d == 1)
                    cx += i;

                // prevent going to the same orb again
                if (_visited[(INDEX_OF(cx, cy))] < 2) {
                    if (d == 0) {
                        // horizontal so fill vertically
                        floodfill(combo, cx + 1, cy, orb, false);
                        floodfill(combo, cx - 1, cy, orb, false);
                    } else {
                        floodfill(combo, cx, cy + 1, orb, false);
                        floodfill(combo, cx, cy - 1, orb, false);
                    }
                }
            }
        }
    }
}

void Board::eraseOrbs(const std::function<void(const Combo&)>& f) {
    //
    for (int i = _row - 1; i >= 0; i--) {
        for (pint j = 0; j < _column; j++) {
            auto orb = (*this)(i, j);
            // ignore empty orbs
            if (orb == pad::empty)
                continue;

            Combo combo(orb);
            floodfill(combo, i, j, orb, true);
            if (combo.loc.size() >= _minErase) {
                f(combo);
            }
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
    newBoard.set(_row, _column, _minErase);
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
