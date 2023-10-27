#pragma once

#include "Deserializer.h"
#include "../Message/IMessage.h"
#include "../Message/TextMessage.h"
#include "../Message/FileMessage.h"
#include "../Message/RequestMessage.h"
#include "../helpers/UtilString.h"

typedef size_t MSG_FIELD_SIZE_TYPE;

inline Deserializer& operator>>(Deserializer&  d, MSG_FIELD_SIZE_TYPE& size)
{
	char data[sizeof(MSG_FIELD_SIZE_TYPE)];
	for (size_t i = 0; i < sizeof(MSG_FIELD_SIZE_TYPE); ++i)
	{
		data[i] = d.GetChar();
	}
	size = *((MSG_FIELD_SIZE_TYPE*)data);

	return d;
}

inline Deserializer& operator>>(Deserializer& d, std::string& str)
{
	MSG_FIELD_SIZE_TYPE size;
	d >> size;

	for (size_t i = 0; i < size; ++i)
	{
		str.push_back(d.GetChar());
	}

	return d;
}

inline Deserializer& operator>>(Deserializer& d, IMessage*& m)
{
	std::string type;
	for (size_t i = 0; i < SIZE_OF_FORMAT; ++i)
	{
		type.push_back(d.GetChar());
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
		while ((c = d.GetChar()) != ' ')
		{
			message.push_back(c);
		}
		message.push_back('\0');
		const std::string& filename = join(split(std::string(message.data()), "/"), "\\");
		m = new RequestMessage(filename);
	}

	if (m)
	{
		m->Deserialize(d);
	}

	return d;
}