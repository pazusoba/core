#include "bridge.hpp"

Bridge::Bridge(QObject *parent) : QObject(parent)
{
    // Path is different for all platforms so let's just use a string instead
    solver = PSolver("RHBDDRRGHDGBHGBGHHRLLRGBBHHRLL", 3, 25, 6000);
    m_row = solver.row;
    m_column = solver.column;
    m_initialBoard = solver.board.getBoardOrbs();
}

void Bridge::solve()
{
    // Don't call it multiple times
    // TODO: add async to this so that UI will not freeze
    if (m_routes.empty())
    {
        // Just call solve and save all solutions
        m_routes = solver.solve();
    }

    m_bestBoard = m_routes[0].getBoardOrbs();
}
