#pragma once

#include <memory>

#include "Event.h"
#include "../Message/IMessagePack.h"
#include "../Message/MessagePack.h"

class MessagesUpdateEvent : public Event
{
public:
	MessagesUpdateEvent(int client_num, const IMessage& msg);
	virtual ~MessagesUpdateEvent() override = default;
	virtual eventType GetEventType() const override;
	int GetClientNum() const;
	const IMessage& GetMsg() const;

private:
	int _client_num;
	const IMessage& _msg;
};
