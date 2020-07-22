#include "bridge.hpp"

Bridge::Bridge(QObject *parent) : QObject(parent)
{
    m_msg = "Hello World from C++";
}
