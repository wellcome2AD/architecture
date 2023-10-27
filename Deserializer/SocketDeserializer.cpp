#include "SocketDeserializer.h"
#include "../helpers/Socket.h"

SocketDeserializer::SocketDeserializer(Socket* s) : _socket(s), _offset(0), _count(0) {}

void SocketDeserializer::Recv()
{
	//�������� _socket->recv() � ����� ����� ������ ���� _count, _offset.
	_count = _socket->recv();
	_offset = 0;
}

char SocketDeserializer::GetChar()

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
