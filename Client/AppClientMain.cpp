#include <vector>
#include <fstream>
#include <sstream>

#include "Appclient.h"
#include "../Message/Message.h"
#include "../helpers/UtilString.h"
#include "../helpers/UtilFile.h"

static std::string getFormat(const std::string& msg) {
	std::string format;
	if (fileExists(msg))
	{
		auto dot_befor_ext = msg.find_last_of('.');
		auto extension = msg.substr(dot_befor_ext);
		size_t ext_size = extension.size();
		format = "file " + std::string((char*)(&ext_size), sizeof(size_t)) + extension;
	}
	else
	{
		format = "text ";
	}
	return format;
}

static std::string readFromFile(const std::string& fileName) {
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

int main()//(int argc, char* argv[])
{
	/*if (argc < 5 || split(argv[3], ":").size() != 2)
	{
		printf("USAGE: Client.exe <USER_NAME> <PASSWORD> <HOST:PORT> <MESSAGE>\n\n");
		printf("Example: Client.exe user 1234 192.168.1.1:12345 Hello world!\n");
		return 0;
	}*/

	int argc = 6;
	char argv[][20] = {"Client.exe", "user", "1234", "192.168.1.1:12345", "Hello", "world!\n"};

	std::string user_name = argv[1], password = argv[2], address = argv[3];
	std::string msg = argv[4];
	for (int i = 5; i < argc; i++)
	{
		msg += std::string(" ") + argv[i];
	}

	// format of message with authorization and some file data is: 
	// userNameLnegth userName file <size_t length of extention><extention><file data>

	// format of text message with authorization is:
	// text <text that was input by user>

	Message msg_to_send;
	size_t username_len = user_name.size();
	msg_to_send.username = std::string((char*)(&username_len), sizeof(size_t)) + user_name;
	
	size_t password_len = password.size();
	msg_to_send.password = std::string((char*)(&password_len), sizeof(size_t)) + password;

	msg_to_send.format = getFormat(msg);

	auto temp = readFromFile(msg);
	if (temp.size() != 0) {
		msg_to_send.message = temp;
	}
	else
	{
		msg_to_send.message = msg;
	}
	
	std::ostringstream str_stream;
	str_stream << msg_to_send;
	Client().send(address, str_stream.str());
}