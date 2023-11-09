#include "SocketSerializer.h"
#include "../helpers/Socket.h"

SocketSerializer::SocketSerializer(Socket* s) : _socket(s) {}

void SocketSerializer::PutChar(char c)
{
	_buffer.push_back(c);
}

void SocketSerializer::Flush()
{
	_socket->send(_buffer.data(), _buffer.size());
	_buffer.clear();
}
