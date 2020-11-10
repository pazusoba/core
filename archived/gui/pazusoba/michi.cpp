#include "michi.h"
#include "../../core/pad.h"

QRoute::QRoute(Route &r)
{
    // Convert a route to a qt route
    m_board = r.getBoardOrbs();

    // Get all info about this route and convert them into a single string
    auto boardInfo = QString("%1 - %2 steps\n").arg(r.getScore()).arg(r.getStep());
    auto location = r.getStartLocation();
    boardInfo += QString("(%1, %2) ").arg(location.first).arg(location.second);
    for (const auto &d : r.getDirections())
    {
        boardInfo += QString::fromStdString(pad::DIRECTION_NAMES[d]) + " ";
    }
    m_info = boardInfo;
}
