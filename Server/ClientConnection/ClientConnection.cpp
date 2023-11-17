#include <thread>

#include "ClientConnection.h"
#include "../../Deserializer/SocketDeserializer.h"
#include "../../Deserializer/DeserializerOperators.h"
#include "../../Serializer/SocketSerializer.h"
#include "../../Serializer/SerializerOperators.h"
#include "../../Message/IMessage.h"
#include "../../Observer/IObserver.h"
#include "../../Observer/MessagesUpdateEvent.h"
#include "../../Observer/ConnResetEvent.h"
#include "../../helpers/Socket/ConnResetException.h"

ClientConnection::ClientConnection(std::shared_ptr<Socket> client, int number) : _client(client), _number(number)
{
	std::thread thr([&]() {
		std::shared_ptr<Socket> client_copy(client);
		SocketDeserializer r(client_copy.get());
		while (true)
		{
			std::shared_ptr<IMessage> msg = nullptr;
			try
			{
				r >> msg;
				Notify(MessagesUpdateEvent(number, *msg));
			}
			catch (const ConnResetException& ex)
			{
				printf("Client %d: ", _number);
				printExc(ex);
				Notify(ConnResetEvent(_number));
				break;
			}
			catch (const std::exception& ex)
			{
			}			
		}
	});
	thr.detach();
}

int ClientConnection::GetNumber() const
{
	return _number;
}

void ClientConnection::sendStr(const std::string& str)
{
	_client->sendStr(str);
}

void ClientConnection::SendMsg(const IMessage& msg)
{
	try
	{
		SocketSerializer s(_client.get());
		s << msg << my_endl();
	}
	catch (const ConnResetException& ex)
	{
		printf("Client %d: ", _number);
		printExc(ex);
		Notify(ConnResetEvent(_number));
	}
	catch (const std::exception& ex)
	{
	}
}

void ClientConnection::AddObserver(IObserver* o)
{
	_observers.push_back(o);
}

void ClientConnection::Notify(const Event& e) // TODO: вызывать Notify, если соединение прервано
{
	for (auto&& o : _observers)
	{
		o->Update(e);
	}
}
