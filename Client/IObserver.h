#pragma once

#include <memory>

#include "Event.h"

class IObserver
{
public:
	virtual ~IObserver() = default;
	virtual void Update(std::shared_ptr<Event> e) = 0;
};
