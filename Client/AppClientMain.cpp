#include <vector>
#include <fstream>
#include <sstream>

#include "Appclient.h"
#include "../Message/AuthorizedMessage.h"
#include "../Message/TextMessage.h"
#include "../Message/FileMessage.h"
#include "../Writer/Writer.h"
#include "../helpers/UtilFile.h"
#include "../helpers/UtilString.h"

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

int main(int argc, char* argv[])
{
	if (argc < 5 || split(argv[3], ":").size() != 2)
	{
		printf("USAGE: Client.exe <USER_NAME> <PASSWORD> <HOST:PORT> <MESSAGE>\n\n");
		printf("Example: Client.exe user 1234 192.168.1.1:12345 Hello world!\n");
		return 0;
	}

	// int argc = 5;
	// char argv[][20] = {"Client.exe", "user", "1234", "192.168.1.1:12345", "Hello"};

	std::string user_name = argv[1], password = argv[2], address = argv[3];
	std::string msg = argv[4];
	for (int i = 5; i < argc; i++)
	{
		msg += std::string(" ") + argv[i];
	}
	
	AuthorizedMessage* msg_to_send = nullptr;
	format msg_format = fileExists(msg) ? file : text;
	switch (msg_format) {
	case text:
	{
		msg_to_send = new TextMessage(user_name, password, msg);
		break;
	}
	case file:
	{
		auto dot_befor_ext = msg.find_last_of('.');
		auto extension = msg.substr(dot_befor_ext);
		auto file_data = readFromFile(msg);
		msg_to_send = new FileMessage(user_name, password, extension, file_data);
	}
	}	
	Client().send(address, msg_to_send);
}