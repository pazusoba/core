#include <fmt/color.h>
#include <pazusoba/board.h>
#include <pazusoba/hash.h>

namespace pazusoba {
void board::printBoard(PrintStyle style) const {
    for (int i = 0; i < size; i++) {
        auto orb = int(this->internalBoard[i]);
        if (i > 0 && i % column == 0) {
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

std::string board::getFormattedBoard(FormatStyle style) const {
    std::string boardString = "";
    for (int i = 0; i < size; i++) {
        auto orb = int(this->internalBoard[i]);
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

size_t board::hash() {
    unsigned char boardString[MAX_BOARD_SIZE] = {'\0'};
    for (int i = 0; i < size; i++) {
        boardString[i] = (unsigned char)(this->internalBoard[i]);
    }
    return hash::djb2_hash(boardString);
}

void board::validateIndex(size_t index) {
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
}

orb& board::operator[](size_t index) {
    validateIndex(index);
    return this->internalBoard[index];
}

orb& board::operator()(size_t x, size_t y) {
    auto index = x * row + y;
    validateIndex(index);
    return this->internalBoard[index];
}
}  // namespace pazusoba
