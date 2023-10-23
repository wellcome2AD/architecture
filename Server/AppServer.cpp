#define USER_DOESNT_EXIST 0
#define INCORRECT_PASSWORD 1
#define CORRECT_USER_AND_PASSWORD 2

#include <string>
#include <process.h>
#include <windows.h>
#include <fstream>
#include <algorithm>

#include "AppServer.h"
#include "../helpers/UtilString.h"
#include "../helpers/UtilFile.h"

bool Server::init(int port)
{
	fileWriteStr(std::string("resources\\ALIVE") + toStr(_getpid()), ""); // pet the watchdog
	if (!m_socket.init(1000))
	{
		return false;
	}

	bool result = false;
	if (result = m_socket.listen(port))
	{
		CreateDirectory("resources", NULL);
		fileWriteExclusive("resources\\CREATED", toStr(m_socket.port()) + "," + toStr(_getpid()));
		printf("server%d listen port %d\n", _getpid(), m_socket.port());
		synchState();
	}
	else if(time(NULL) - last_synch_time >= 10)
	{
		last_synch_time = time(NULL);
		synchState();		
	}

	if (users.size() == 0) 
	{
		loadUsers();
	}
	if (rights.size() == 0)
	{
		loadRights();
	}

	return result;
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

		const std::vector<std::string>& tokens = split(data, " ");
		if (tokens[0] == "GET")
		{
			client->sendStr(handleRequest(tokens));
		}
		else
		{
			Message m = Message::Parse(data, n);
			handleMessage(m);
		}
	}
}

void Server::synchState()
{
	auto state_file = fopen("resources\\STATE", "r");
	if (state_file)
	{
		fseek(state_file, offset, SEEK_SET);
		std::vector<char> data;
		while (!feof(state_file))
		{
			data.push_back(fgetc(state_file));
		}
		if (!data.empty())
		{
			data.pop_back();
		}
		offset += (long)data.size();
		for (std::string& line : split(std::string(data.data(), data.size()), "\n"))
		{
			if (!line.empty())
			{
				m_data.push_back(line);
			}
		}
		fclose(state_file);
	}
}

void Server::loadUsers()
{
	// открыть файл USERS
	// прочитать все данные в map, где ключ - первое слово в строке, значение - второе
	auto users_file = fopen("resources\\USERS", "r");
	if (users_file)
	{
		std::vector<char> data;
		while (!feof(users_file))
		{
			data.push_back(fgetc(users_file));
		}
		if (!data.empty())
		{
			data.pop_back();
		}
		size_t str_num = 0;
		for (std::string& line : split(std::string(data.data(), data.size()), "\n"))
		{
			if (!line.empty())
			{
				auto params = split(line, " ");
				if (params.size() != 2) {
					printf("Error: incorrect data format in file USERS in string %zd\n", str_num);
					printf("Format should be: <user name> <password>\n\n");
					return;
				}
				users[params[0]] = params[1];
				++str_num;
			}
		}
		fclose(users_file);
	}
}

void Server::loadRights()
{
	// открыть файл USERS
	// прочитать все данные в map, где ключ - первое слово в строке, значение - вектор из второго слова, расколотого на токены через запятые
	auto rights_file = fopen("resources\\RIGHTS", "r");
	if (rights_file)
	{
		std::vector<char> data;
		while (!feof(rights_file))
		{
			data.push_back(fgetc(rights_file));
		}
		if (!data.empty())
		{
			data.pop_back();
		}
		size_t str_num = 0;
		for (std::string& line : split(std::string(data.data(), data.size()), "\n"))
		{
			if (!line.empty())
			{
				auto params = split(line, " ");
				if (params.size() != 2) {
					printf("Error: incorrect data format in file RIGHTS in string %zd\n", str_num);
					printf("Format should be: <user name> <message type 1>,<message type 2>\n\n");
					return;
				}
				rights[params[0]] = split(params[1], ",");
				++str_num;
			}
		}
		fclose(rights_file);
	}
}

int Server::userExists(const std::string& userName, const std::string& password) const
{
	auto iter = users.find(userName);
	int result = iter == users.end();
	if (result != USER_DOESNT_EXIST)
	{
		result = iter->second == password ? CORRECT_USER_AND_PASSWORD : INCORRECT_PASSWORD;
	}
	return result;
}

bool Server::checkRights(const std::string& userName, const std::string& msgType) const
{
	auto iter = rights.find(userName);
	bool user_has_right = iter != rights.end() && std::find(iter->second.begin(), iter->second.end(), msgType) != iter->second.end();
	return user_has_right;
}

void Server::handleMessage(const Message& m)
{
	if (userExists(m.username, m.password) != 2) {
		printf("User %s with password %s doesn't exist\n", m.username.c_str(), m.password.c_str());
		return;
	}
	if (m.format.find("text") != std::string::npos) // if data is a text
	{
		if (!checkRights(m.username, "text"))
		{
			printf("User %s can't send text messages\n", m.username.c_str());
			return;
		}
		printf("-----RECV-----\n%s\n--------------\n", m.message.c_str());
		fflush(stdout);
		m_data.push_back(m.message); // store it in the feed
	}
	else
	{
		if (!checkRights(m.username, "file"))
		{
			printf("User %s can't send file\n", m.username.c_str());
			return;
		}
		size_t index_of_ext_length = m.format.find(" ") + 1;
		size_t ext_length = *((size_t*)(&m.format[index_of_ext_length]));
		size_t index_of_ext = index_of_ext_length + sizeof(size_t);
		std::string ext(&m.format[index_of_ext], ext_length);
		printf("-----RECV-----\n%s %s\n--------------\n", ext.c_str(), m.format.c_str());
		fflush(stdout);

		std::string fileName = createUniqueFileName(ext.c_str());
		std::ofstream file("resources/" + fileName, std::ios::binary);
		size_t begin_of_data = index_of_ext + ext_length;
		if (file.is_open())
		{
			std::copy(m.message.begin(), m.message.end(), std::ostreambuf_iterator<char>(file));
			m_data.push_back(fileName); // store it in the feed
		}
		else
		{
			printf("Can't open file %s\n", ("resources/" + fileName).c_str());
		}
	}
	fileAppend("resources\\STATE", m_data.back() + "\n"); // store it in the file for subsequent runs
}

std::string Server::handleRequest(const std::vector<std::string>& tokens)
{
	std::string response;
	auto endsWith = [](const std::string& fileName, const std::string& ext) {
		return fileName.substr(fileName.size() - ext.size()) == ext;
	};
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
		response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + toStr((int)payload.size()) + "\r\n\r\n" + payload + end;
	}
	else if (fileExists("resources/" + filename) && (endsWith(filename, ".png") || endsWith(filename, ".jpg") || endsWith(filename, ".ico")))
	{
		std::ifstream file("resources/" + filename, std::ios::binary);
		if (file.is_open())
		{
			std::string fileData(std::istreambuf_iterator<char>(file), {});
			//client->sendStr("HTTP/1.1 200 OK\r\nContent-Type: image/png\r\nContent-Length: " + std::to_string(fileData.size()) + "\r\n\r\n" + fileData);
			response = "HTTP/1.1 200 OK\r\nContent-Type: image/png\r\nContent-Length: " + std::to_string(fileData.size()) + "\r\n\r\n" + fileData;
		}
		else
		{
			printf("Can't open file\n");
		}
	}
	else
	{
		response = "HTTP/1.1 404 Not Found\r\n\r\n";
	}
	return response;
}
