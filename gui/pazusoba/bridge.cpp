#include "bridge.h"

QBridge::QBridge(QObject *parent) : QObject(parent)
{
    // Path is different for all platforms so let's just use a string instead
    solver = PSolver(3, 25, 1000);
    m_row = solver.row;
    m_column = solver.column;
    m_initialBoard = solver.board.getBoardOrbs();
}

void QBridge::solve()
{
    // Don't call it multiple times
    // TODO: add async to this so that UI will not freeze
    if (m_routes.empty())
    {
        // Just call solve and save all solutions
        for (Route &s : solver.solve())
        {
            m_routes.append(new QRoute(s));
        }

        // Notify out fancy UI
        routeChanged();
    }
}
