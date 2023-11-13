#pragma once

enum eventType
{
	messagesUpdate
};

class Event
{
public:
	virtual ~Event() = default;
	virtual eventType GetEventType() const = 0;
};
