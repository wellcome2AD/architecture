#pragma once

enum eventType
{
	messagesUpdate, clientDisconnect
};

class Event
{
public:
	virtual ~Event() = default;
	virtual eventType GetEventType() const = 0;
};
