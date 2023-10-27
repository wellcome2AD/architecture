#pragma once

#include <vector>

#include "Serialiser.h"

class Socket;

class SocketSerialiser : public Serialiser
{
private:
	Socket* _socket;
	std::vector<char> _buffer;

public:
	SocketSerialiser(Socket* s);
	virtual void PutChar(char c) override;
	virtual void Flush() override;
};
