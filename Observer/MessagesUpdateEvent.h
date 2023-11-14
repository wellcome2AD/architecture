#pragma once

#include <memory>

#include "Event.h"
#include "../Message/IMessagePack.h"

class MessagesUpdateEvent : public Event
{
public:
	MessagesUpdateEvent() = default;
	virtual ~MessagesUpdateEvent() override = default;
	virtual eventType GetEventType() const override;
};
