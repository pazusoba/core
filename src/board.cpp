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

void Board::eraseCombo(VisitedIndex* visited, Combo& combo, pint ox, pint oy) {
    // assume this index is valid
    auto orb_index = INDEX_OF(ox, oy);
    auto orb = (*this)[orb_index];
    _queue.emplace_front(orb_index);  // 25%

    while (!_queue.empty()) {
        auto currIndex = _queue.front();
        _queue.pop_front();
        if (visited[currIndex].visited)
            continue;
        else
            visited[currIndex].visited = true;

        pint x = currIndex / _column;
        pint y = currIndex % _column;

        // Check vertically from x
        // go up from x
        bool vup = true;
        pint vupIndex = x;
        while (vup) {
            pint cx = vupIndex - 1;
            if (cx >= _row) {
                vup = false;
            } else if ((*this)(cx, y) == orb) {
                vupIndex--;
            } else {
                vup = false;
            }
        }
        // go down from x
        bool vdown = true;
        pint vdownIndex = x;
        while (vdown) {
            pint cx = vdownIndex + 1;
            if (cx >= _row) {
                vdown = false;
            } else if ((*this)(cx, y) == orb) {
                vdownIndex++;
            } else {
                vdown = false;
            }
        }

        // as long as three orbs are connected, it can be erased
        // min erase doesn't matter at all here
        bool vErase = vdownIndex - vupIndex >= 2;
        for (pint i = vupIndex; i <= vdownIndex; i++) {
            auto currIndex = INDEX_OF(i, y);
            if (vErase) {
                // add this location and clear the orb
                combo.loc.insert(currIndex);  // 7.8%
                _board[currIndex] = 0;
                // to be visited
                if (i != x)
                    _queue.emplace_front(currIndex);
            } else {
                // simply go and visit it
                _queue.emplace_front(currIndex);
            }
        }

        // Check horizontally from y
        // go left from y
        bool hleft = true;
        pint hleftIndex = y;
        while (hleft) {
            pint cy = hleftIndex - 1;
            if (cy >= _column) {
                hleft = false;
            } else if ((*this)(x, cy) == orb) {
                hleftIndex--;
            } else {
                hleft = false;
            }
        }
        // go right from y
        bool hright = true;
        pint hrightIndex = y;
        while (hright) {
            pint cy = hrightIndex + 1;
            if (cy >= _column) {
                hright = false;
            } else if ((*this)(x, cy) == orb) {
                hrightIndex++;
            } else {
                hright = false;
            }
        }

        bool hErase = hrightIndex - hleftIndex >= 2;
        for (pint i = hleftIndex; i <= hrightIndex; i++) {
            auto currIndex = INDEX_OF(x, i);
            if (hErase) {
                combo.loc.insert(currIndex);  // 7.8%
                _board[currIndex] = 0;
                if (i != y)
                    _queue.emplace_front(currIndex);
            } else {
                _queue.emplace_front(currIndex);
            }
        }
    }
}

void Board::eraseOrbs(ComboList& list) {
    VisitedIndex* erased = new VisitedIndex[_size];

    for (pint x = 0; x < _row; x++) {
        for (pint y = 0; y < _column; y++) {
            auto orb = (*this)(x, y);
            // ignore empty orbs
            if (orb == pad::empty)
                continue;

            Combo combo(orb);
            // 58%, can be optimised, improving eraseCombo can increase
            // the speed very significantly
            eraseCombo(erased, combo, x, y);
            if (combo.loc.size() >= _minErase) {
                list.push_front(combo);  // 24% here, can be optimized
                // maybe a callback not sure how
            }
        }
    }

    delete[] erased;
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
            case dawnglare:
                boardString += pad::ORB_WEB_NAME[orb];
                break;
            case serizawa:
                boardString += pad::ORB_WEB_MAKER_NAME[orb];
                break;
            case padopt:
                boardString += pad::ORB_WEB_PADOPT_NAME[orb];
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

// This is right because x and y are both unsigned
// so we don't need to handle negative values
orb& Board::operator()(pint x, pint y) {
    auto index = INDEX_OF(x, y);
    return (*this)[index];
}
}  // namespace pazusoba
