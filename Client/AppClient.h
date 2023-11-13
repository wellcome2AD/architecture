#pragma once

#include <vector>
#include <string>
#include <memory>

#include "IObservable.h"
#include "helpers/SocketClient.h"
#include "../Message/AuthorizedMessage.h"

// command-line Client app that can send messages to Server.
class Client : public IObservable
{
public:
	Client() = default;
	virtual ~Client() = default;
	bool connect(std::string url);
	void disconnect();
	bool send(const std::string& url, const AuthorizedMessage* msg);
	void recv();
	virtual void AddObserver(std::shared_ptr<IObserver> o) override;
	virtual void RemoveObserver(std::shared_ptr<IObserver> o) override;
	virtual void Notify(std::shared_ptr<Event> e) override;

private:
	std::unique_ptr<SocketClient> _s;
	std::vector<std::shared_ptr<IObserver>> _observers;
};
