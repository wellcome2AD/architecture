#pragma once

#include <string>
#include <iostream>

class Message {
public:
	Message() = default;
	Message(const char* data, int n)
	{
		size_t cur_index = 0;
		auto username_len = *((size_t*)(&data[cur_index]));
		cur_index += sizeof(size_t);
		username = std::string(&data[cur_index], username_len);
		cur_index += username_len;

		auto password_len = *((size_t*)(&data[cur_index]));
		cur_index += sizeof(size_t);
		password = std::string(&data[cur_index], password_len);
		cur_index += password_len;

		format = std::string(&data[cur_index], std::find(&data[cur_index], data + n, ' '));

		cur_index += format.size() + 1;
		message = std::string(&data[cur_index]);
	}

public:
	std::string username, password, format, message;
};

inline std::ostream& operator<<(std::ostream& os, const Message& m) {
	os << m.username << m.password << m.format << m.message;
	return os;
}
