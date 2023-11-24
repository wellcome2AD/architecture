#pragma once

#include <sstream>
#include <memory>
#include <mutex>

#include "../Client/AppClient.h"
#include "../Observer/IObserver.h"
#include "../Message/IMessagePack.h"
#include "../Monitor/helpers/Console.h"

class Viewer : public IObserver
{
public:
	static void Run();

private:
	Viewer();	
	void tryToConnect();
	void sendMsg(const std::string& username, const std::string& password, const std::string& msg);
	void printMsgs() const;	
	virtual void Update(const Event& e) override;

private:
	std::stringstream _buffer;

	friend Client;
	std::string _url;
	Client _client;
	std::atomic<bool> is_connected;

	std::shared_ptr<IMessagePack> _msg_pack;

	mutable std::mutex _console_mutex;
	Console m_console;
};
