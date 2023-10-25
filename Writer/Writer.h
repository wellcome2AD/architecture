#pragma once

#include <string>

#include "../helpers/Socket.h"
#include "../Message/IMessage.h"
#include "../Message/TextMessage.h"
#include "../Message/FileMessage.h"
#include "../Message/RequestMessage.h"

typedef size_t MSG_FIELD_SIZE_TYPE;

class my_endl{};

class Writer
{
private:
	Socket* _socket;
	std::vector<char> _buffer;

public:
	Writer(Socket* s) : _socket(s) {}

	Writer& operator<<(my_endl endl)
	{
		send();
		return *this;
	}

	Writer& operator<<(MSG_FIELD_SIZE_TYPE size)
	{
		auto size_str = std::string((char*)(&size), sizeof(MSG_FIELD_SIZE_TYPE));
		_buffer.insert(_buffer.end(), size_str.begin(), size_str.end());
		return *this;
	}

	Writer& operator<<(const std::string& str)
	{
		MSG_FIELD_SIZE_TYPE size = str.size();
		*this << size;
		_buffer.insert(_buffer.end(), str.begin(), str.end());
		return *this;
	}

	Writer& operator<<(const IMessage* m)
	{
		if (!m)
			return *this;
		std::string format;
		switch (m->GetFormat()) {
		case text:
		{
			format = "text";
			break;
		}
		case file:
		{
			format = "file";
			break;
		}
		}
		_buffer.insert(_buffer.end(), format.begin(), format.end());
		m->Write(this);

		return *this;
	}

private:
	void send()
	{
		_socket->send(_buffer.data(), _buffer.size());
		_buffer.clear();
	}
};
