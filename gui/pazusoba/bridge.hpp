#ifndef BRIDGE_HPP
#define BRIDGE_HPP

#include <QObject>

class Bridge : public QObject
{
    Q_OBJECT
public:
    explicit Bridge(QObject *parent = nullptr);

signals:

};

#endif // BRIDGE_HPP
