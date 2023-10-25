#define USER_DOESNT_EXIST 0
#define INCORRECT_PASSWORD 1
#define CORRECT_USER_AND_PASSWORD 2

#include <string>
#include <process.h>
#include <windows.h>
#include <fstream>
#include <algorithm>

#include "AppServer.h"
#include "../Reader/Reader.h"
#include "../helpers/UtilString.h"
#include "../helpers/UtilFile.h"
#include "../Message/IMessage.h"
#include "../Message/FileMessage.h"
#include "../Message/RequestMessage.h"

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
	else if (time(NULL) - last_synch_time >= 10)
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

		Reader r(&*client);
		IMessage* msg = nullptr;
		r >> msg;
		handleMessage(msg);
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
				auto temp = split(line, " "); // split by space on user and message
				m_data.emplace(temp[0], temp[1]);
			}
		}
		fclose(state_file);
	}
}

void Server::loadUsers()
{
	// ������� ���� USERS
	// ��������� ��� ������ � map, ��� ���� - ������ ����� � ������, �������� - ������
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
	// ������� ���� RIGHTS
	// ��������� ��� ������ � map, ��� ���� - ������ ����� � ������, �������� - ������ �� ������� �����, ����������� �� ������ ����� �������
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
	int result = iter == users.end() ? USER_DOESNT_EXIST : INCORRECT_PASSWORD;
	if (result != USER_DOESNT_EXIST)
	{
		result = iter->second == password ? CORRECT_USER_AND_PASSWORD : INCORRECT_PASSWORD;
	}
	return result;
}

bool Server::checkRights(const std::string& userName, format msgType) const
{
	auto iter = rights.find(userName);
	auto find_res = std::find(iter->second.begin(), iter->second.end(), toString(msgType));
	bool user_has_right = iter != rights.end() && find_res != iter->second.end();
	return user_has_right;
}

void Server::handleMessage(IMessage* m)
{
	if (m->GetFormat() == getReq)
		handleRequest(dynamic_cast<RequestMessage*>(m));
	else
		handleAuthorizedMessage(dynamic_cast<AuthorizedMessage*>(m));
}

void Server::handleAuthorizedMessage(AuthorizedMessage* m)
{
	if (userExists(m->GetUsername(), m->GetPassword()) != 2) {
		printf("User %s with password %s doesn't exist\n", m->GetUsername().c_str(), m->GetPassword().c_str());
		return;
	}
	if (!checkRights(m->GetUsername(), m->GetFormat()))
	{
		printf("User %s can't send %s messages\n", m->GetUsername().c_str(), toString(m->GetFormat()).c_str());
		return;
	}
	std::string data_to_store;
	switch (m->GetFormat()) {
	case text:
	{
		data_to_store = m->GetMsg();
		break;
	}
	case file:
	{
		std::string fileName = createUniqueFileName((dynamic_cast<FileMessage*>(m))->GetExtension().c_str());
		std::ofstream file("resources/" + fileName, std::ios::binary);
		if (file.is_open())
		{
			auto temp = m->GetMsg();
			std::copy(temp.begin(), temp.end(), std::ostreambuf_iterator<char>(file));
			data_to_store = fileName;
		}
		else
		{
			printf("Can't open file %s\n", ("resources/" + fileName).c_str());
			return;
		}
		break;
	}
	}
	printf("-----RECV-----\n%s %s\n--------------\n\n", m->GetUsername().c_str(), data_to_store.c_str());
	fflush(stdout);
	m_data.emplace(m->GetUsername(), data_to_store); // store it in the feed
	fileAppend("resources\\STATE", m->GetUsername() + " " + data_to_store + "\r\n"); // store it in the file for subsequent runs
}

std::string Server::handleRequest(RequestMessage* m)
{
	std::string response;
	auto endsWith = [](const std::string& fileName, const std::string& ext) {
		return fileName.substr(fileName.size() - ext.size()) == ext;
	};
	auto&& filename = m->GetMsg();
	if (filename == "\\")
	{ // main entry point (e.g. http://localhost:12345/)
		std::string payload = "<!DOCTYPE html>"\
			"<html>" \
			"<head>" \
			"<meta charset = \"UTF-8\">" \
			"<title>Server Content</title>" \
			"</head>" \
			"<body>\n";
		for (auto&& [user, msg] : m_data)
		{
			if (fileExists("resources/" + msg) && (endsWith(msg, ".png") || endsWith(msg, ".jpg")))
			{
				payload += "<br><p>" + user + "<br><img src=\"/" + msg + "\"></p>\n";
			}
			else
			{
				payload += (user + " " + msg + "<br>"); // collect all the feed and send it back to browser
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
