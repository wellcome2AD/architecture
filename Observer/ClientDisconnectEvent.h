#pragma once

#include <memory>

#include "Event.h"
#include "../Message/IMessagePack.h"

class ClientDisconnectEvent : public Event
{
public:
	ClientDisconnectEvent(int client_num);
	virtual ~ClientDisconnectEvent() override = default;
	virtual eventType GetEventType() const override;
	int GetNumber() const;

private:
	int _client_num;
};
