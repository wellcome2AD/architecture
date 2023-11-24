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
	{
		if (v.m_console.isEscapePressed()) // pressing ESC activates typing mode
		{
			std::lock_guard<std::mutex> lg(v._console_mutex);
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
			v.sendMsg(username, password, message);
			std::cout << std::endl;
		}
	}
}

Viewer::Viewer()
{
	_msg_pack = std::make_shared<MessagePack>();
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
		break;
	}
	default:
		assert(0);
		break;
	}
	_client.send(_url, msg_to_send);
	delete[] msg_to_send;
}

void Viewer::printMsgs() const
{
	assert(_msg_pack);
	if (_msg_pack->GetMsgs().size() > 0)
	{
		std::cout << std::string(14, '-') << std::endl;
		std::cout << "MESSAGES:\n";
		std::cout << std::string(14, '-') << std::endl;
		for (auto&& msg : _msg_pack->GetMsgs())
		{
			switch (msg->GetFormat())
			{
			case text:
			{
				auto&& text_msg = static_cast<const TextMessage&>(*msg);
				std::cout << text_msg.GetUsername() << " : " << text_msg.GetMsg() << std::endl;
				break;
			}
			case file:
			{
				auto&& file_msg = static_cast<const FileMessage&>(*msg);
				auto&& file_name = file_msg.GetMsg();
				std::cout << file_msg.GetUsername() << " : " << file_name << std::endl;				
				break;
			}
			default:
				assert(0);
			}
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

void Viewer::handleMessage(const IMessage& msg)
{
	switch (msg.GetFormat())
	{
	case text:
	{
		auto&& txt_msg = static_cast<const TextMessage&>(msg);
		_msg_pack->AddMsg(txt_msg);
		break;
	}
	case file:
	{
		auto&& file_msg = static_cast<const FileMessage&>(msg);
		auto&& file_name = createUniqueFileName(file_msg.GetExtension().c_str());
		fileWrite(file_name, file_msg.GetMsg().data(), file_msg.GetMsg().size());
		system(std::string("start " + file_name).c_str());
		// TODO хранить в FileMessage не расширение, а имя файла
		_msg_pack->AddMsg(FileMessage(file_msg.GetUsername(), file_msg.GetPassword(), file_msg.GetExtension(), file_name));
		break;
	}
	case msgPack:
	{
		auto&& pack_msg = static_cast<const MessagePack&>(msg);
		for (auto&& m : pack_msg.GetMsgs())
		{
			handleMessage(*m);
		}
		break;
	}
	default:
		assert(0);
	}
}

void Viewer::Update(const Event& e)
{
	switch (e.GetEventType())
	{
	case messagesUpdate:
	{
		auto&& msg = static_cast<const MessagesUpdateEvent&>(e).GetMsg();
		handleMessage(msg);
		std::lock_guard<std::mutex> console_lg(_console_mutex);
		m_console.clearScreen();
		printf("Press ESC to type message\n");
		printMsgs();		
		printf("----------\nPress ESC to type message\n");		
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
