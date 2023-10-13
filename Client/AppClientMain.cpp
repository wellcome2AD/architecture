#include <vector>
#include <fstream>

#include "Appclient.h"
#include "helpers/UtilString.h"
#include "helpers/UtilFile.h"

int main(int argc, char* argv[])
{	
	if (argc < 3 || split(argv[1], ":").size() != 2)
	{
		printf("USAGE: Client.exe <HOST:PORT> <MESSAGE>\n\n");
		printf("Example: Client.exe 192.168.1.1:12345 Hello world!\n");
		return 0;
	}

	std::string msg = argv[2];
	for (int i = 3; i < argc; i++)
		msg += std::string(" ") + argv[i];
	
	Client c;
	//std::string msg = "image.png";
	std::string format;

	// format of message with some file data is: 
	// file <size_t length of extention><extention><file data>

	// format of text message is:
	// text <text that was input by user>

	if (fileExists(msg))
	{
		auto dot_befor_ext = msg.find_last_of('.');
		auto extension = msg.substr(dot_befor_ext);
		size_t ext_size = extension.size();
		format = "file " + std::string((char *)(&ext_size), sizeof(size_t)) + extension;
		
		std::string filename = msg;
		std::ifstream file(filename, std::ios::binary);
		if (file.is_open())
		{
			msg = std::string(std::istreambuf_iterator<char>(file), {});
		}
		else
		{
			printf("Error\n");
		}
	}
	else
	{
		format = "text ";
	}
	
	msg.insert(msg.begin(), format.begin(), format.end());
	c.send(argv[1], msg);

	/*const char* data = msg.c_str();
	int n = msg.size();
	
	{
		std::string format(data, std::find(data, data + n, ' '));
		size_t index_of_ext_length = format.size() + 1;
		size_t ext_length = *((size_t*)(&data[index_of_ext_length]));
		size_t index_of_ext = index_of_ext_length + sizeof(size_t);
		std::string ext(&data[index_of_ext], ext_length);
		printf("%s %s\n", ext.c_str(), format.c_str());
	}
	*/
}