#include <winsock2.h>

#include "SocketSerializer.h"
#include "../helpers/Socket.h"
#include "SerializerOperators.h"

SocketSerializer::SocketSerializer(Socket* s) : _socket(s) {}

SocketSerializer::~SocketSerializer()
{
	*this << my_endl();
}


void SocketSerializer::PutChar(char c)
{
	_buffer.push_back(c);
}

class SocketException : public std::exception
{
public:
	SocketException(int er_code) {
		char* s_ptr = NULL;
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, er_code,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR)&s, 0, NULL);
		s = std::string(s_ptr);
		LocalFree(s_ptr);
	}
	virtual const char* what() const override
	{
		return s.c_str();
	}

private:
	std::string s;
};

void SocketSerializer::Flush()
{
	if (!_buffer.empty())
	{
		auto return_val = _socket->send(_buffer.data(), _buffer.size());
		if (return_val == SOCKET_ERROR)
		{
			auto er_code = WSAGetLastError();
			throw SocketException(er_code);
		}
		_buffer.clear();
	}
}
