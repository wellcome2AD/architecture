#pragma once

#include <string>
#include <memory>

#include "../Client/AppClient.h"
#include "../Client/IObserver.h"
#include "../Message/IMessagePack.h"

class Viewer : public IObserver
{
public:
	Viewer(std::string url);
	void SetUrl(std::string url);
	const std::shared_ptr<IMessagePack>& GetMsgs() const;

private:
	virtual void Update(std::shared_ptr<Event> e) override;
	void tryToConnect();

private:
	friend Client;
	std::string _url;
	Client _client;
	std::atomic<bool> is_connected;
	std::shared_ptr<IMessagePack> _msg_pack;
};

