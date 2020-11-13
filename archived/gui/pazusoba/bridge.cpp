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
    if (shouldSolve)
    {
        // Clear all old routes
        m_routes.clear();
        // Just call solve and save all solutions
        for (Route &s : solver.solve())
        {
            m_routes.append(new QRoute(s));
        }

        // Notify out fancy UI
        routeChanged();
        shouldSolve = false;
    }
}

void QBridge::random()
{
    solver.setRandomBoard(m_row, m_column);
    // Update saved board as well
    m_initialBoard = solver.board.getBoardOrbs();
    // Clear all routes
    m_routes.clear();
    shouldSolve = true;
    initialBoardChanged();
}

void QBridge::updateBeam(int index)
{
    solver.setBeamSize(m_beamList[index].toInt());
    shouldSolve = true;
}

void QBridge::updateStep(int index)
{
    solver.setStepLimit(m_stepList[index].toInt());
    shouldSolve = true;
}
