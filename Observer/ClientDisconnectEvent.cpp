#include "ClientDisconnectEvent.h"

ClientDisconnectEvent::ClientDisconnectEvent(int client_num)
    : _client_num(client_num)
{}

eventType ClientDisconnectEvent::GetEventType() const
{
    return clientDisconnect;
}

int ClientDisconnectEvent::GetNumber() const
{
    return _client_num;
}
