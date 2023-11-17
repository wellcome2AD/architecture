#include "ConnResetEvent.h"

ConnResetEvent::ConnResetEvent(int con_num)
    : _con_num(con_num)
{}

eventType ConnResetEvent::GetEventType() const
{
    return connReset;
}

int ConnResetEvent::GetNumber() const
{
    return _con_num;
}
