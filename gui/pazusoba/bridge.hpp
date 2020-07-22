#ifndef BRIDGE_HPP
#define BRIDGE_HPP

#include <QObject>
#include "../../lib/solver.hpp"

class Bridge : public QObject
{
    Q_OBJECT

    // Properties
    Q_PROPERTY(QString message READ getMessage)

public:
    explicit Bridge(QObject *parent = nullptr);
    QString getMessage() const
        { return m_msg; }
signals:

private:
    QString m_msg;
};

#endif // BRIDGE_HPP
