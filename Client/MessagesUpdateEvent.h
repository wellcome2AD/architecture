#pragma once

#include <memory>

#include "Event.h"
#include "../Message/IMessagePack.h"

class MessagesUpdateEvent : public Event
{
public:
	MessagesUpdateEvent(std::shared_ptr<IMessagePack> msg_pack);
	virtual eventType GetEventType() const override;
	std::shared_ptr<IMessagePack> GetMsgs() const;

private:
	std::shared_ptr<IMessagePack> _msg_pack;
};
