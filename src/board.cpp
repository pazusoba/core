#include <fmt/color.h>
#include <pazusoba/board.h>

namespace pazusoba {
board::board() {
    this->internalBoard.fill(-1);
}

void board::printBoard(boardPrintStyles style) const {
    for (int i = 0; i < size; i++) {
        auto orb = int(this->internalBoard[i]);
        if (i > 0 && i % column == 0) {
            fmt::print("\n");
            if (style == boardPrintStyles::colourful) {
                fmt::print("\n");
            }
        }

        switch (style) {
            case boardPrintStyles::test:
                fmt::print("{} ", orb);
                break;
            case boardPrintStyles::name:
                fmt::print("{}\t", constant::orbNames[orb]);
                break;
            case boardPrintStyles::colourful:
                // bg -> background, fg -> foreground
                fmt::print(bg(constant::orbColors[orb]), "  ");
                fmt::print("  ");
                break;
        }
    }
    fmt::print("\n============\n");
}

std::string board::getFormattedBoard(boardFormatStyles style) const {
    return "";
}

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
