#pragma once

#include "Deserialiser.h"
#include "../Message/IMessage.h"
#include "../Message/TextMessage.h"
#include "../Message/FileMessage.h"
#include "../Message/RequestMessage.h"
#include "../helpers/UtilString.h"

typedef size_t MSG_FIELD_SIZE_TYPE;

inline Deserialiser& operator>>(Deserialiser&  d, MSG_FIELD_SIZE_TYPE& size)
{
	char data[sizeof(MSG_FIELD_SIZE_TYPE)];
	for (size_t i = 0; i < sizeof(MSG_FIELD_SIZE_TYPE); ++i)
	{
		data[i] = getchar();
	}
	size = *((MSG_FIELD_SIZE_TYPE*)data);

	return d;
}

inline Deserialiser& operator>>(Deserialiser& d, std::string& str)
{
	MSG_FIELD_SIZE_TYPE size;
	d >> size;

	for (size_t i = 0; i < size; ++i)
	{
		str.push_back(getchar());
	}

	return d;
}

inline Deserialiser& operator>>(Deserialiser& d, IMessage*& m)
{
	std::string type;
	for (size_t i = 0; i < SIZE_OF_FORMAT; ++i)
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
		m->Deserialise(d);
	}

	return d;
}