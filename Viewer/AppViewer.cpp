#include <iostream>
#include <thread>

#include "AppViewer.h"
#include "../Observer/MessagesUpdateEvent.h"
#include "../Message/IMessagePack.h"
#include "../Message/TextMessage.h"

Viewer::Viewer(std::string url) : _url(url) {
	_client.AddObserver(this);
	tryToConnect();
}

void Viewer::SetUrl(std::string url)
{
	_url = url;
	is_connected.store(false);
	_client.disconnect();
	tryToConnect();
}

const std::shared_ptr<IMessagePack>& Viewer::GetMsgs() const
{
	return _msg_pack;
}

void Viewer::Update(const Event& e)
{
	switch (e.GetEventType())
	{
	case messagesUpdate:
	{
		_msg_pack = _client.GetMsgs();
		std::cout << std::string(14, '-') << std::endl;
		for (auto&& msg : _msg_pack.get()->GetMsgs())
		{
			auto&& text_msg = dynamic_cast<TextMessage*>(msg.get());
			std::cout << text_msg->GetUsername() << " : " << text_msg->GetMsg() << std::endl;
		}
		std::cout << std::string(14, '-') << std::endl;
		std::cout << std::endl;
		break;
	}
	case clientDisconnect:
	{
		std::cout << "Server disconnected\n";
		is_connected.store(false); // TODO: предложить ввести другой url дл€ присоединени€
		break;
	}
	default:
		assert(0);
		break;
	}
}

void Viewer::tryToConnect()
{
	is_connected.store(_client.connect(_url));
	if (is_connected.load())
	{
		std::thread thr([&]() {
			while (is_connected.load())
			{
				_client.recv();
			}
		});
		thr.detach();
	}
	else
	{
		std::cout << "Error: Can't connect by this URL\n\n";
	}
}
