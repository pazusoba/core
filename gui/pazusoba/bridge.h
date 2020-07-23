#ifndef QBRIDGE_HPP
#define QBRIDGE_HPP

#include <QObject>
#include <vector>
#include "../../lib/solver.hpp"

class QBridge : public QObject
{
    Q_OBJECT

    // Properties
    Q_PROPERTY(int row MEMBER m_row CONSTANT)
    Q_PROPERTY(int column MEMBER m_column CONSTANT)
    Q_PROPERTY(std::vector<int> initialBoard MEMBER m_initialBoard CONSTANT)
    Q_PROPERTY(std::vector<int> bestBoard MEMBER m_bestBoard CONSTANT)

public:
    explicit QBridge(QObject *parent = nullptr);
signals:

public slots:
    void solve();
private:
    // Define the size of the board
    int m_row, m_column;
    // This is the initial board for display in the beginning
    std::vector<int> m_initialBoard;
    std::vector<int> m_bestBoard;
    // All routes
    std::vector<Route> m_routes;

    // Let's just pass in some default parameter first
    PSolver solver;
};

#endif // BRIDGE_HPP
