#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <process.h>
#include <windows.h>
#include <fstream>

#include "AppServer.h"
#include "../helpers/UtilString.h"
#include "../helpers/UtilFile.h"

bool Server::init(int port)
{
	if (!m_socket.init(1000) || !m_socket.listen(port))
		return false;

	CreateDirectory("resources", NULL);

	if (!fileWriteExclusive("resources\\CREATED", toStr(m_socket.port()) + "," + toStr(_getpid())))
		return false;

	printf("server started: port %d, pid %d\n", m_socket.port(), _getpid());

	char* state = fileReadStr("resources\\STATE"); // load state from previous run
	if (state)
	{
		for (std::string& line : split(state, "\n"))
			if (!line.empty())
				m_data.push_back(line);
		delete[] state;
	}

	return true;
}

void Server::run()
{
	while (true)
	{
		fileWriteStr(std::string("resources\\ALIVE") + toStr(_getpid()), ""); // pet the watchdog
		std::shared_ptr<Socket> client = m_socket.accept(); // accept incoming connection

		if (!client->isValid())
		{
			continue;
		}

		int n = client->recv(); // receive data from the connection, if any
		char* data = client->data();

		if (n <= 0)
		{
			printf("-----RECV-----\n%s\n--------------\n", "Error");
			continue;
		}

		std::string format(data, std::find(data, data + n, ' '));
		if (format.find("GET") == std::string::npos) // if data is not a GET-request from browser
		{
			if (format.find("text") != std::string::npos) // if data is a text
			{
				printf("-----RECV-----\n%s\n--------------\n", data);
				fflush(stdout);
				m_data.push_back(data); // store it in the feed
			}
			else
			{
				size_t index_of_ext_length = format.size() + 1;
				size_t ext_length = *((size_t*)(&data[index_of_ext_length]));
				size_t index_of_ext = index_of_ext_length + sizeof(size_t);
				std::string ext(&data[index_of_ext], ext_length);
				printf("-----RECV-----\n%s %s\n--------------\n", ext.c_str(), format.c_str());
				fflush(stdout);
				
				std::string fileName = createUniqueFileName(ext.c_str());
				std::ofstream file("resources/" + fileName, std::ios::binary);
				size_t begin_of_data = index_of_ext + ext_length;
				if (file.is_open())
				{
					std::copy(&data[begin_of_data], data + n, std::ostreambuf_iterator<char>(file));
					m_data.push_back(fileName); // store it in the feed
				}
				else
				{
					printf("Can't open file %s\n", ("resources/" + fileName).c_str());
				}
			}			
			fileAppend("resources\\STATE", m_data.back() + "\n"); // store it in the file for subsequent runs
		}
		else
		{
			const std::vector<std::string>& tokens = split(data, " ");
			auto endsWith = [](const std::string& fileName, const std::string& ext) {
				return fileName.substr(fileName.size() - ext.size()) == ext;
			};
			if (tokens.size() >= 2 && tokens[0] == "GET") // this is browser's request
			{
				// convert URL to file system path, e.g. request to img/1.png resource becomes request to .\img\1.png file in Server's directory tree
				const std::string& filename = join(split(tokens[1], "/"), "\\");
				if (filename == "\\")
				{ // main entry point (e.g. http://localhost:12345/)
					std::string payload = "<!DOCTYPE html>"\
										  "<html>" \
										  "<head>" \
										  "<meta charset = \"UTF-8\">" \
										  "<title>Server Content</title>" \
										  "</head>" \
										  "<body>\n";
					for (auto& s : m_data)
					{						
						if (fileExists("resources/" + s) && (endsWith(s, ".png") || endsWith(s, ".jpg")))
						{							
							payload += "<p><img src=\"/" + s + "\"></p>\n";
						}
						else
						{
							payload += (s + "<br>"); // collect all the feed and send it back to browser
						}
					}
					std::string end = "</body>\n</html>";
					client->sendStr("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + toStr((int)payload.size()) + "\r\n\r\n" + payload + end);
				}
				else if (fileExists("resources/" + filename) && (endsWith(filename, ".png") || endsWith(filename, ".jpg") || endsWith(filename, ".ico")))
				{
					std::ifstream file("resources/" + filename, std::ios::binary);
					if (file.is_open())
					{
						std::string fileData(std::istreambuf_iterator<char>(file), {});
						client->sendStr("HTTP/1.1 200 OK\r\nContent-Type: image/png\r\nContent-Length: " + std::to_string(fileData.size()) + "\r\n\r\n" + fileData);
					}
					else
					{
						printf("Can't open file\n");
					}
				}
				else
				{
					client->sendStr("HTTP/1.1 404 Not Found\r\n\r\n");
				}
			}
		}
	}
}
