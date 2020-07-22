#include "bridge.hpp"

Bridge::Bridge(QObject *parent) : QObject(parent)
{
    solver = PSolver("D:\\Windows\\Programming\\AI\\pad-solver\\assets\\board_76_b97e4a.txt", 3, 25, 1000);
    m_row = solver.row;
    m_column = solver.column;
    m_initialBoard = solver.board.getBoardOrbs();
}
