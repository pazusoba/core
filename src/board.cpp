#include <pazusoba/board.h>

namespace pazusoba {
board::board() {
    this->internalBoard.fill(-1);
}

void board::printBoard(boardPrintStyles style) const {
    for (int i = 0; i < size; i++) {
        switch (style) {
            case boardPrintStyles::test:
                break;
            case boardPrintStyles::name:
                break;
            case boardPrintStyles::colourful:
                break;
        }
    }
}

std::string board::getFormattedBoard(boardPrintStyles style) const {}

orb& board::operator[](size_t index) {
    if (size > 0 && (int)index >= size) {
        // If the board is 6x5, the index should never go over 29,
        // this can be treated as a test as well.
        throw std::out_of_range(fmt::format(
            "pazusoba::board index {} out of range {}", index, size - 1));
    } else if (index >= MAX_BOARD_SIZE) {
        throw std::out_of_range(
            fmt::format("pazusoba::board index {} great than max board size {}",
                        index, MAX_BOARD_SIZE));
    }
    return this->internalBoard[index];
}
}  // namespace pazusoba
