#include "michi.h"

QRoute::QRoute(Route &r)
{
    // Convert a route to a qt route
    m_board = r.getBoardOrbs();

    // Get all info about this route and convert them into a single string
    auto boardInfo = QString("%1 - %2\n").arg(r.getScore(), r.getStep());
    for (const auto &d : r.getDirections())
    {
        boardInfo += QString("%1 ").arg(d);
    }
    m_info = boardInfo;
}
