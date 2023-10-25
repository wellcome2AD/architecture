#pragma once

#include <string>

#include "../helpers/Socket.h"
#include "../Message/IMessage.h"
#include "../Message/TextMessage.h"
#include "../Message/FileMessage.h"
#include "../Message/RequestMessage.h"

typedef size_t FIELDS_TYPE;

class Reader
{
private:
	Socket *_socket;
	size_t _offset;
	size_t _count;

public:
	Reader(Socket *s) : _socket(s), _offset(0), _count(0) {}

	Reader& operator>>(FIELDS_TYPE& size)
	{
		char data[sizeof(FIELDS_TYPE)];
		for (size_t i = 0; i < sizeof(FIELDS_TYPE); ++i)
		{
			data[i] = getchar();
		}
		size = *(FIELDS_TYPE*)(data);

		return *this;
	}

	Reader& operator>>(std::string& str)
	{
		FIELDS_TYPE size;
		*this >> size;

		for (size_t i = 0; i < size; ++i)
		{
			str.push_back(getchar());
		}

		return *this;
	}

	Reader& operator>>(IMessage*& m)
	{
		std::string type;
		char c = getchar();
		for(size_t i = 0; i < SIZE_OF_FORMAT; ++i, c = getchar())
		{
			type.push_back(c);
		}
		if (type.find("text") != std::string::npos)
		{
			m = new TextMessage();
		}
		else if (type.find("file") != std::string::npos)
		{
			m = new FileMessage();
		}
		else if (type.find("GET") != std::string::npos)
		{
			char c;
			std::vector<char> message;
			while ((c = getchar()) != ' ')
			{
				message.push_back(c);
			}
			m = new RequestMessage(std::string(message.data()));
		}

		if (m)
		{
			m->Read(this);
		}
		
		return *this;
	}

private:
	void recv()
	{
		//вызываем _socket->recv() и после этого меняем поля _count, _offset.
		_count = _socket->recv();
	}

	char getchar()
	{
		char res = _socket->data()[_offset];
		++_offset;
		if (_offset == _count)
		{
			recv();
		}
		return res;
	}
};
