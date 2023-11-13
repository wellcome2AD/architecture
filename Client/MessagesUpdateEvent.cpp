#include "MessagesUpdateEvent.h"

MessagesUpdateEvent::MessagesUpdateEvent(std::shared_ptr<IMessagePack> msg_pack)
	: _msg_pack(msg_pack)
{}

eventType MessagesUpdateEvent::GetEventType() const
{
	return messagesUpdate;
}

std::shared_ptr<IMessagePack> MessagesUpdateEvent::GetMsgs() const
{
	return _msg_pack;
}
