#include <winsock2.h>

#include "SocketSerializer.h"
#include "../helpers/Socket.h"
#include "SerializerOperators.h"
#include "../helpers/SocketException.h"

SocketSerializer::SocketSerializer(Socket* s) : _socket(s) {}

SocketSerializer::~SocketSerializer()
{
	*this << my_endl();
}

void SocketSerializer::PutChar(char c)
{
	_buffer.push_back(c);
}

void SocketSerializer::Flush()
{
	if (!_buffer.empty())
	{
		auto return_val = _socket->send(_buffer.data(), _buffer.size());
		if (return_val == SOCKET_ERROR)
		{
			auto er_code = WSAGetLastError();
			throw SocketException(er_code);
		}
		_buffer.clear();
	}
}
