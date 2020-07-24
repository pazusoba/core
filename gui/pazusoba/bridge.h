#ifndef Q_BRIDGE_H
#define Q_BRIDGE_H

#include <QObject>
#include <QVector>
#include "../../lib/solver.h"
#include "michi.h"

class QBridge : public QObject
{
    Q_OBJECT

    // Properties
    Q_PROPERTY(int row MEMBER m_row CONSTANT)
    Q_PROPERTY(int column MEMBER m_column CONSTANT)
    Q_PROPERTY(std::vector<int> initialBoard MEMBER m_initialBoard CONSTANT)
    Q_PROPERTY(QList<QRoute *> routes MEMBER m_routes NOTIFY routeChanged)

public:
    explicit QBridge(QObject *parent = nullptr);

signals:
    void routeChanged();

public slots:
    void solve();

private:
    // Define the size of the board
    int m_row, m_column;
    // This is the initial board for display in the beginning
    std::vector<int> m_initialBoard;
    // All routes
    QList<QRoute *> m_routes;

    // Let's just pass in some default parameter first
    PSolver solver;
};

#endif // Q_BRIDGE_H
