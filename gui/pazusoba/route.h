#ifndef Q_ROUTE_H
#define Q_ROUTE_H

#include <QObject>
#include "../../lib/route.h"

class QRoute : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString info MEMBER m_info CONSTANT)
    Q_PROPERTY(std::vector board MEMBER m_board CONSTANT)

public:
    explicit QRoute(QObject *parent = nullptr) : QObject(parent) {}
    void convert(Route &r);

signals:

private:
    // The UI only needs a board for the grid and a string for the list
    std::vector<int> m_board;
    QString m_info;
};

#endif // QROUTE_H
