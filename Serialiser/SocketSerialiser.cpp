#include "SocketSerialiser.h"
#include "../helpers/Socket.h"

SocketSerialiser::SocketSerialiser(Socket* s) : _socket(s) {}

void SocketSerialiser::PutChar(char c)
{
	_buffer.push_back(c);
}

void SocketSerialiser::Flush()
{
	_socket->send(_buffer.data(), _buffer.size());
	_buffer.clear();
}
