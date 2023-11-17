#include <iostream>
#include <thread>
#include <cstdio>

#include "AppViewer.h"
#include "../Observer/MessagesUpdateEvent.h"
#include "../Message/IMessagePack.h"
#include "../Message/TextMessage.h"
#include "../Message/FileMessage.h"
#include "../helpers/Socket/ConnResetException.h"
#include "../helpers/UtilFile.h"

void Viewer::printMenu()
{
	std::cout << std::string(10, '-') + '\n';
	std::cout << "MENU:\n";
	std::cout << "1. Send message\n";
	std::cout << "2. Read messages\n";
	std::cout << "3. Exit\n";
	std::cout << std::string(10, '-') + '\n';
	std::cout << "Input command: ";
	int command;
	std::cin >> command;
	switch (command)
	{
	case 1:
	{
		std::cout << "Input username: ";
		std::string username;
		std::cin >> username;
		std::cout << "Input password: ";
		std::string password;
		std::cin >> password;		
		getchar();
		std::cout << "Input message: ";
		std::string message;
		std::getline(std::cin, message);
		sendMsg(username, password, message);
		std::cout << std::endl;
		printMenu();
		return;
		break;
	}
	case 2:
	{
		printMsgs();
		std::cout << std::endl;
		printMenu();
		return;
		break;
	}
	case 3:
	{
		exit(EXIT_SUCCESS);
		is_connected.store(false);
		break;
	}
	default:
	{
		std::cout << "Incorrect command: " << command << std::endl;
		std::cout << std::endl;
		printMenu();
		break;
	}
	}
}

void Viewer::sendMsg(const std::string& username, const std::string& password, const std::string& msg)
{
	AuthorizedMessage* msg_to_send = nullptr;
	format msg_format = fileExists(msg) ? file : text;
	switch (msg_format) {
	case text:
	{
		msg_to_send = new TextMessage(username, password, msg);
		break;
	}
	case file:
	{
		auto dot_befor_ext = msg.find_last_of('.');
		auto extension = msg.substr(dot_befor_ext);
		auto file_data = readFromFile(msg);
		msg_to_send = new FileMessage(username, password, extension, file_data);
	}
	default:
		assert(0);
		break;
	}
	_client.send(_url, msg_to_send);
	delete[] msg_to_send;
}

void Viewer::Run()
{
	Viewer v;
	std::cout << "Welcome to app Viewer!\n";

	v.is_connected.store(false);

	while (v.is_connected.load() == false)
	{
		v.tryToConnect();

		if (!v.is_connected.load())
		{
			std::cout << "Error: Can't connect by this URL\n\n";
		}
	}
	v._client.AddObserver(&v);

	std::thread thr([&]() {
		while (v.is_connected.load())
		{
			v._client.recv();
		}
		});
	thr.detach();

	while (true)
		v.printMenu();
}

void Viewer::printMsgs() const
{
	if (_msg_pack.get() && _msg_pack.get()->GetMsgs().size() > 0)
	{
		std::lock_guard<std::mutex> lg(_msg_mutex);
		std::cout << std::string(14, '-') << std::endl;
		std::cout << "\nMESSAGES:\n";
		std::cout << std::string(14, '-') << std::endl;
		for (auto&& msg : _msg_pack.get()->GetMsgs())
		{
			auto&& text_msg = dynamic_cast<TextMessage*>(msg.get());
			std::cout << text_msg->GetUsername() << " : " << text_msg->GetMsg() << std::endl;
		}
		std::cout << std::string(14, '-') << std::endl;
	}
	else
	{
		std::cout << std::string(14, '-') << std::endl;
		std::cout << "NO NEW MESSAGES\n";
		std::cout << std::string(14, '-') << std::endl;
	}
}

void Viewer::Update(const Event& e)
{
	switch (e.GetEventType())
	{
	case messagesUpdate:
	{
		std::lock_guard<std::mutex> lg(_msg_mutex);
		_msg_pack = _client.getMsgs();
		break;
	}
	case connReset:
	{
		std::cout << "Server disconnected\n";
		is_connected.store(false);
		tryToConnect();
		break;
	}
	default:
		assert(0);
		break;
	}
}

void Viewer::tryToConnect()
{
	std::string ip("127.0.0.1:");
	std::cout << "Input port to connect: " << ip;
	std::string port;
	std::cin >> port;
	_url = ip + port;
	is_connected.store(_client.connect(_url));
}
