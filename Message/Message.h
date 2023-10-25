#pragma once

#define SIZE_OF_FORMAT 4

#include <string>
#include <iostream>
#include <fstream>

#include "../helpers/UtilFile.h"

class Message {
public:
	Message(const std::string& username, const std::string& password, const std::string& message)
	{
		// format of message with authorization and some file data is: 
		// <size_t userNameLength><userName><size_t passwordLength><password>file <size_t length of extention><extention><file data>

		// format of text message with authorization is:
		// <size_t userNameLength><userName><size_t passwordLength><password>text <text that was input by user>

		size_t username_len = username.size();
		this->username = std::string((char*)(&username_len), sizeof(size_t)) + username;

		size_t password_len = password.size();
		this->password = std::string((char*)(&password_len), sizeof(size_t)) + password;

		format = getFormat(message);

		auto temp = readFromFile(message);
		if (temp.size() != 0) {
			this->message = temp;
		}
		else
		{
			this->message = message;
		}
	}
	static Message Parse(const char* data, int n)
	{
		Message result;
		size_t cur_index = 0;
		auto username_len = *((size_t*)(&data[cur_index]));
		cur_index += sizeof(size_t);
		result.username = std::string(&data[cur_index], username_len);
		cur_index += username_len;

		auto password_len = *((size_t*)(&data[cur_index]));
		cur_index += sizeof(size_t);
		result.password = std::string(&data[cur_index], password_len);
		cur_index += password_len;

		result.format = std::string(&data[cur_index], SIZE_OF_FORMAT);
		cur_index += result.format.size();

		if (result.format.find("file") != std::string::npos)
		{			
			auto ext_len = *((size_t*)(&data[cur_index]));
			cur_index += sizeof(size_t);
			result.file_ext = std::string(&data[cur_index], ext_len);
			cur_index += result.file_ext.size();
		}

		result.message = std::string(&data[cur_index], data + n);

		return result;
	}

public:
	std::string username, password, format, file_ext, message;

private:
	Message() = default;

	std::string getFormat(const std::string& msg) {
		std::string format;
		if (fileExists(msg))
		{
			auto dot_befor_ext = msg.find_last_of('.');
			auto extension = msg.substr(dot_befor_ext);
			size_t ext_size = extension.size();
			format = "file";
			file_ext = std::string((char*)(&ext_size), sizeof(size_t)) + extension;
		}
		else
		{
			format = "text";
			file_ext = std::string();
		}
		return format;
	}

	std::string readFromFile(const std::string& fileName) {
		std::string data;
		if (fileExists(fileName)) {
			std::ifstream file(fileName, std::ios::binary);
			if (file.is_open())
			{
				data = std::string(std::istreambuf_iterator<char>(file), {});
			}
			else
			{
				printf("Error\n");
			}
		}
		return data;
	}
};

inline std::ostream& operator<<(std::ostream& os, const Message& m) {
	os << m.username << m.password << m.format;
	if (m.file_ext.size() != 0)
	{
		os << m.file_ext;
	}
	os << m.message;
	return os;
}
