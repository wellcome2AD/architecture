#include "SocketDeserialiser.h"
#include "../helpers/Socket.h"

SocketDeserialiser::SocketDeserialiser(Socket* s) : _socket(s), _offset(0), _count(0) {}

void SocketDeserialiser::Recv()
{
	//вызываем _socket->recv() и после этого меняем поля _count, _offset.
	_count = _socket->recv();
	_offset = 0;
}

char SocketDeserialiser::GetChar()

{
	if (_count == 0)
	{
		Recv();
	}
	char res = _socket->data()[_offset];
	++_offset;
	if (_offset == _count)
	{
		Recv();
	}
	return res;
}
