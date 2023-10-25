#include <vector>
#include <fstream>
#include <sstream>

#include "Appclient.h"
#include "../Message/Message.h"
#include "../helpers/UtilString.h"
#include "../helpers/UtilFile.h"

int main(int argc, char* argv[])
{
	if (argc < 5 || split(argv[3], ":").size() != 2)
	{
		printf("USAGE: Client.exe <USER_NAME> <PASSWORD> <HOST:PORT> <MESSAGE>\n\n");
		printf("Example: Client.exe user 1234 192.168.1.1:12345 Hello world!\n");
		return 0;
	}

	//int argc = 6;
	//char argv[][35] = {"Client.exe", "user", "1234", "192.168.1.1:12345", "D:/GithubFiles/Практика2/image.png"};

	std::string user_name = argv[1], password = argv[2], address = argv[3];
	std::string msg = argv[4];
	for (int i = 5; i < argc; i++)
	{
		msg += std::string(" ") + argv[i];
	}

	Message msg_to_send(user_name, password, msg);	
	std::ostringstream str_stream;
	str_stream << msg_to_send;
	printf("%s ", msg_to_send.format.c_str());
	printf("%zd", msg_to_send.message.size());
	Client().send(address, str_stream.str());
}