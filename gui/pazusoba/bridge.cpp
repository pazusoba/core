#include "bridge.hpp"

Bridge::Bridge(QObject *parent) : QObject(parent)
{
    m_msg = { 1, 2, 3, 4, 5, 6 };
}
