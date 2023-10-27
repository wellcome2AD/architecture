#pragma once

#include "Deserialiser.h"

class Socket;

typedef size_t MSG_FIELD_SIZE_TYPE;

class SocketDeserialiser : public Deserialiser
{
private:
	Socket* _socket;
	size_t _offset;
	size_t _count;

public:
	SocketDeserialiser(Socket* s);
	virtual void Recv() override;
	virtual char GetChar() override;
};
