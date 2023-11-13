#pragma once

#include <exception>
#include <string>

class SocketException : public std::exception
{
public:
	SocketException(int er_code) {
		char* s_ptr = NULL;
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, er_code,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR)&s_ptr, 0, NULL);
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
