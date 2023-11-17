#pragma once

#include <sstream>
#include <memory>
#include <mutex>

#include "../Client/AppClient.h"
#include "../Observer/IObserver.h"
#include "../Message/IMessagePack.h"

class Viewer : public IObserver
{
public:
	static void Run();

private:
	Viewer() = default;
	virtual void Update(const Event& e) override;
	void tryToConnect();
	void printMenu();
	void printMsgs() const;
	void sendMsg(const std::string& username, const std::string& password, const std::string& msg);

private:
	std::stringstream _buffer;

	friend Client;
	std::string _url;
	Client _client;
	std::atomic<bool> is_connected;

	mutable std::mutex _msg_mutex;
	std::shared_ptr<IMessagePack> _msg_pack;
};
