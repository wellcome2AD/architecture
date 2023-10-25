#pragma once

#include <string>

#include "../helpers/Socket.h"
#include "../Message/IMessage.h"
#include "../Message/TextMessage.h"
#include "../Message/FileMessage.h"
#include "../Message/RequestMessage.h"
#include "../helpers/UtilString.h"

typedef size_t MSG_FIELD_SIZE_TYPE;

class Reader
{
private:
	Socket *_socket;
	size_t _offset;
	size_t _count;

public:
	Reader(Socket *s) : _socket(s), _offset(0), _count(0) {}

	Reader& operator>>(MSG_FIELD_SIZE_TYPE& size)
	{
		char data[sizeof(MSG_FIELD_SIZE_TYPE)];
		for (size_t i = 0; i < sizeof(MSG_FIELD_SIZE_TYPE); ++i)
		{
			data[i] = getchar();
		}
		size = *((MSG_FIELD_SIZE_TYPE*)data);

		return *this;
	}

	Reader& operator>>(std::string& str)
	{
		MSG_FIELD_SIZE_TYPE size;
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
		for(size_t i = 0; i < SIZE_OF_FORMAT; ++i)
		{
			type.push_back(getchar());
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
			message.push_back('\0');
			const std::string& filename = join(split(std::string(message.data()), "/"), "\\");
			m = new RequestMessage(filename);
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
		_offset = 0;
	}

	char getchar()
	{
		if (_count == 0)
		{
			recv();
		}
		char res = _socket->data()[_offset];
		++_offset;
		if (_offset == _count)
		{
			recv();
		}
		return res;
	}
};
