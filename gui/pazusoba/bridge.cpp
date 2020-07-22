#include "bridge.hpp"

Bridge::Bridge(QObject *parent) : QObject(parent)
{
    m_row = 5;
    m_column = 6;
    solver = PSolver("D:\\Windows\\Programming\\AI\\pad-solver\\assets\\sample_board_65.txt", 3, 25, 1000);
    m_initialBoard = solver.board.getBoardOrbs();
}
