#ifndef BRIDGE_HPP
#define BRIDGE_HPP

#include <QObject>
#include "../../lib/solver.hpp"

class Bridge : public QObject
{
    Q_OBJECT

    // Properties
    Q_PROPERTY(QString message READ getHelloWorld)

public:
    explicit Bridge(QObject *parent = nullptr);
signals:

private:
    QString getHelloWorld()
    {
        return "Hello WOrld";
    }
};

#endif // BRIDGE_HPP
