#pragma once

#include <string>

#include "Serialiser.h"
#include "../Message/IMessage.h"
#include "../Message/TextMessage.h"
#include "../Message/FileMessage.h"
#include "../Message/RequestMessage.h"

typedef size_t MSG_FIELD_SIZE_TYPE;
class my_endl {};

inline Serialiser& operator<<(Serialiser& s, my_endl endl)
{
	s.Flush();
	return s;
}

inline Serialiser& operator<<(Serialiser& s, MSG_FIELD_SIZE_TYPE size)
{
	auto size_str = std::string((char*)(&size), sizeof(MSG_FIELD_SIZE_TYPE));
	//_buffer.insert(_buffer.end(), size_str.begin(), size_str.end());
	for(size_t i = 0; i < size_str.size(); ++i)
	{
		s.PutChar(size_str[i]);
	}
	return s;
}

inline Serialiser& operator<<(Serialiser& s, const std::string& str)
{
	MSG_FIELD_SIZE_TYPE size = str.size();
	s << size;
	//_buffer.insert(_buffer.end(), str.begin(), str.end());
	for(size_t i = 0; i < size; ++i)
	{
		s.PutChar(str[i]);
	}
	return s;
}

inline Serialiser& operator<<(Serialiser& s, const IMessage* m)
{
	if (!m)
	{
		return s;
	}
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
	//_buffer.insert(_buffer.end(), format.begin(), format.end());
	for(size_t i = 0; i < format.size(); ++i)
	{
		s.PutChar(format[i]);
	}
	m->Serialise(s);
	return s;
}