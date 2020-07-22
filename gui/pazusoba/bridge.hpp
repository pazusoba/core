#ifndef BRIDGE_HPP
#define BRIDGE_HPP

#include <QObject>
#include <vector>
#include "../../lib/solver.hpp"

class Bridge : public QObject
{
    Q_OBJECT

    // Properties
    Q_PROPERTY(std::vector<int> message READ getMessage)

public:
    explicit Bridge(QObject *parent = nullptr);
    std::vector<int> getMessage() const
        { return m_msg; }
signals:

private:
    std::vector<int> m_msg;
};

#endif // BRIDGE_HPP
