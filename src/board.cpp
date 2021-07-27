#include <pazusoba/board.h>

namespace pazusoba {
board::board() {
    this->internalBoard.fill(-1);
}

orb& board::operator[](size_t index) {
    if (index >= MAX_BOARD_SIZE) {
        throw std::out_of_range("Index out of range");
    }
    return this->internalBoard[index];
}
}  // namespace pazusoba
