#ifndef Q_ROUTE_H
#define Q_ROUTE_H

#include <QObject>
#include <vector>
#include "../../lib/route.h"

class QRoute
{
    Q_PROPERTY(QString info MEMBER m_info CONSTANT)
    Q_PROPERTY(std::vector<int> board MEMBER m_board CONSTANT)

public:
    QRoute(Route &r);

private:
    // The UI only needs a board for the grid and a string for the list
    std::vector<int> m_board;
    QString m_info;
};

#endif // Q_ROUTE_H
