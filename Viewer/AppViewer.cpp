#include <iostream>
#include <thread>

#include "AppViewer.h"
#include "../Client/MessagesUpdateEvent.h"
#include "../Message/IMessagePack.h"
#include "../Message/TextMessage.h"

Viewer::Viewer(std::string url) : _url(url) {
	_client.AddObserver(std::shared_ptr<IObserver>(this));
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

void Viewer::Update(std::shared_ptr<Event> e)
{
	if (e->GetEventType() == messagesUpdate)
	{
		auto&& msg_upd_event = dynamic_cast<MessagesUpdateEvent*>(e.get());
		_msg_pack = msg_upd_event->GetMsgs();
		auto&& msg_pack = _msg_pack.get()->GetMsgs();
		for (auto&& msg : msg_pack)
		{
			auto&& text_msg = dynamic_cast<TextMessage*>(msg.get());
			std::cout << text_msg->GetUsername() << " : " << text_msg->GetMsg() << std::endl;
		}
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
