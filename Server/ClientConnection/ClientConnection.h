#pragma once

#include <memory>

#include "../../Observer/IObservable.h"
#include "../../helpers/socket/Socket.h"

class IMessage;

class ClientConnection : public IObservable
{
public:
	ClientConnection(std::shared_ptr<Socket> client, size_t number);
	size_t GetNumber() const;
	void sendStr(const std::string& str);
	void SendMsg(const IMessage& msg);

	// Унаследовано через IObservable
	virtual void AddObserver(IObserver* o) override;
	virtual void Notify(const Event& e) override;

private:
	std::shared_ptr<Socket> _client;
	std::vector<IObserver*> _observers;
	size_t _number;
};