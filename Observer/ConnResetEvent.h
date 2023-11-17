#pragma once

#include <memory>

#include "Event.h"
#include "../Message/IMessagePack.h"

class ConnResetEvent : public Event
{
public:
	ConnResetEvent(int client_num);
	virtual ~ConnResetEvent() override = default;
	virtual eventType GetEventType() const override;
	int GetNumber() const;

private:
	int _con_num;
};
