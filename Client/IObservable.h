#pragma once

#include <memory>

class IObserver;
class Event;

class IObservable
{
public:
	virtual ~IObservable() = default;
	virtual void AddObserver(std::shared_ptr<IObserver> o) = 0;
	virtual void RemoveObserver(std::shared_ptr<IObserver> o) = 0;
	virtual void Notify(std::shared_ptr<Event> e) = 0;
};
