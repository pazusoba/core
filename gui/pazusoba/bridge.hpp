#ifndef BRIDGE_HPP
#define BRIDGE_HPP

#include <QObject>
#include "../../lib/solver.hpp"

class Bridge : public QObject
{
    Q_OBJECT

    // Properties
    Q_PROPERTY(int row MEMBER m_row CONSTANT)
    Q_PROPERTY(int column MEMBER m_column CONSTANT)

public:
    explicit Bridge(QObject *parent = nullptr);
signals:

private:
    // Define the size of the board
    int m_row, m_column;
    // Let's just pass in some default parameter first
    PSolver solver;
};

#endif // BRIDGE_HPP
