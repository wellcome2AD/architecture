#define USER_DOESNT_EXIST 0
#define INCORRECT_PASSWORD 1
#define CORRECT_USER_AND_PASSWORD 2

#include <string>
#include <process.h>
#include <windows.h>
#include <fstream>
#include <algorithm>

#include "AppServer.h"
#include "../Deserializer/DeserializerOperators.h"
#include "../Deserializer/SocketDeserializer.h"
#include "../Serializer/SerializerOperators.h"
#include "../Serializer/SocketSerializer.h"
#include "../helpers/UtilString.h"
#include "../helpers/UtilFile.h"
#include "../helpers/socket/SocketException.h"
#include "../Message/IMessage.h"
#include "../Message/FileMessage.h"
#include "../Message/RequestMessage.h"
#include "../Message/MessagePack.h"
#include "../helpers/File.h"
#include "ResponseBuilder/ResponseBuilder.h"
#include "../Observer/MessagesUpdateEvent.h"
#include "../Observer/ConnResetEvent.h"
#include "../helpers/Socket/ConnResetException.h"

Server::~Server()
{
	// TODO: остановить поток обработки сообщений из MsgQueue и поток принятия новых соединений
}

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
	auto&& task = [&]() {
		while (true)
		{
			std::shared_ptr<Socket> client = m_socket.accept(); // accept incoming connection
			if (!client->isValid())
			{
				continue;
			}
			printf("\n--------------\n");
			printf("Client %zd connected\n", _clients.size());
			printf("--------------\n");
			auto&& cl_con = std::shared_ptr<ClientConnection>(new ClientConnection(client, _clients.size()));
			cl_con->AddObserver(this);
			std::lock_guard clients_lg(_clients_mutex);
			_clients.push_back(cl_con);

			printf("send to client %zd:\n", cl_con->GetNumber());
			std::lock_guard mdata_lg(m_data_mutex);
			MessagePack msg_pack = convertSContToMsgPack();
			cl_con->SendMsg(msg_pack);
			printf("--------------\n");
		}
	};
	std::thread thr(task);
	thr.detach();

	while (true)
	{
		fileWriteStr(std::string("resources\\ALIVE") + toStr(_getpid()), ""); // pet the watchdog
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
				auto msg = join(temp, " ", 1);
				m_data->push_back(std::make_pair<std::string, std::string>(temp[0].data(), msg.c_str()));
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
	// открыть файл RIGHTS
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

void Server::handleMessage(const IMessage* m, ClientConnection& client)
{
	assert(m);
	switch (m->GetFormat())
	{
	case getReq:
	{
		std::string response = handleRequest(static_cast<const RequestMessage*>(m));
		if (response.size() != 0)
		{
			client.sendStr(response);
			// удаление ClientConnection
			auto&& predicate = [&client](const std::shared_ptr<ClientConnection>& c) {
				return c->GetNumber() == client.GetNumber();
			};
			std::lock_guard lg(_clients_mutex);
			_clients.erase(std::remove_if(_clients.begin(), _clients.end(), predicate), _clients.end());
		}
		break;
	}
	case msgPack:
	{
		auto&& msg_pack = static_cast<const IMessagePack*>(m);
		for (auto&& imsg : msg_pack->GetMsgs())
		{
			auto&& author_msg = static_cast<const AuthorizedMessage&>(*imsg);
			handleAuthorizedMessage(&author_msg);
		}
		break;
	}
	case text:
	case file:
	{
		handleAuthorizedMessage(static_cast<const AuthorizedMessage*>(m));
		break;
	}
	default:
		assert(0);
		break;
	}
}
void Server::handleAuthorizedMessage(const AuthorizedMessage* m)
{
	if (userExists(m->GetUsername(), m->GetPassword()) != 2) {
		printf("User %s with password %s doesn't exist\n\n", m->GetUsername().c_str(), m->GetPassword().c_str());
		return;
	}
	if (!checkRights(m->GetUsername(), m->GetFormat()))
	{
		printf("User %s can't send %s messages\n\n", m->GetUsername().c_str(), toString(m->GetFormat()).c_str());
		return;
	}
	std::string data_to_store;
	std::unique_ptr<AuthorizedMessage> msg;
	switch (m->GetFormat()) {
	case text:
	{
		data_to_store = m->GetMsg();
		msg = std::make_unique<TextMessage>(m->GetUsername(), "", data_to_store.data());
		break;
	}
	case file:
	{
		auto&& file_m = static_cast<const FileMessage*>(m);
		auto&& ext = file_m->GetExtension();
		auto&& file_data = file_m->GetMsg();
		std::string fileName = createUniqueFileName(ext.c_str());
		fileWrite("resources/" + fileName, file_data.c_str(), file_data.size(), true);
		data_to_store = fileName;
		msg = std::make_unique<FileMessage>(m->GetUsername(), "", ext, file_data);
		break;
	}
	default:
		assert(0);
		break;
	}
	std::lock_guard lg(m_data_mutex);
	m_data->push_back(std::make_pair<std::string, std::string>(m->GetUsername(), data_to_store.data())); // store it in the feed
	fileAppend("resources\\STATE", m->GetUsername() + " " + data_to_store + "\r\n"); // store it in the file for subsequent runs
	printf("\n--------------\n");
	printf("send to all clients new message\n");
	MessagePack msg_pack;
	msg_pack.AddMsg(std::move(msg));
	broadcast(msg_pack);
}

std::string Server::handleRequest(const RequestMessage* m)
{
	std::string response;
	auto endsWith = [](const std::string& fileName, const std::string& ext) {
		return fileName.substr(fileName.size() - ext.size()) == ext;
	};
	auto&& root = m->GetMsg();
	ResponseBuilder r_b;
	if (root == "\\")
	{
		r_b.SetCode(200);
		r_b.SetContent(*m_data);
		r_b.SetContentType(text_html);
		response = r_b.Build();
	}
	else {
		auto&& filename = "resources/" + root;
		if (fileExists(filename) && (endsWith(filename, ".png") || endsWith(filename, ".jpg") || endsWith(filename, ".ico")))
		{
			File f(filename);
			r_b.SetCode(200);
			r_b.SetContent(f);
			r_b.SetContentType(image_png);
			response = r_b.Build();
		}
		else
		{
			r_b.SetCode(404);
			response = r_b.Build();
		}
	}
	return response;
}

void Server::broadcast(const IMessagePack& msg_pack)
{
	printf("----BROADCAST----\n");
	auto&& predicate = [&msg_pack](const std::shared_ptr<ClientConnection>& c) {
		printf("send to client %zd new message\n", c->GetNumber());
		try
		{
			c->SendMsg(msg_pack);
		}
		catch (const ConnResetException& ex)
		{
			printExc(ex);
			return false;
		}
		catch (const std::exception&)
		{
			return true;
		}
		printf("SUCCESS\n");
		return false;
	};
	std::lock_guard lg(_clients_mutex);
	_clients.erase(std::remove_if(_clients.begin(), _clients.end(), predicate), _clients.end());
	printf("-----------------\n");
}

MessagePack Server::convertSContToMsgPack() const
{
	MessagePack msg_pack;
	for (auto&& msg : *m_data)
	{
		auto&& user_name = msg.first;
		auto&& message = msg.second;
		printf("%s %s\n", user_name.c_str(), message.c_str());
		std::unique_ptr<AuthorizedMessage> auth_m;
		auto&& file_path = "resources/" + message;
		if (fileExists(file_path))
		{
			File f(file_path);
			auto ext = split(message, ".").back();
			auth_m = std::make_unique<FileMessage>(user_name, "", "." + ext, f.GetData());
		}
		else 
		{
			auth_m = std::make_unique<TextMessage>(user_name, "", message);
		}
		msg_pack.AddMsg(std::move(auth_m));
	}
	return msg_pack;
}

void Server::Update(const Event& e)
{
	switch (e.GetEventType())
	{
	case connReset:
	{
		auto&& event = static_cast<const ConnResetEvent&>(e);
		auto&& client_number = event.GetNumber();
		auto predicate = [&](std::shared_ptr<ClientConnection> c) { return c->GetNumber() == client_number; };
		std::lock_guard lg(_clients_mutex);
		_clients.erase(std::remove_if(_clients.begin(), _clients.end(), predicate), _clients.end());
		break;
	}
	case messagesUpdate:
	{
		auto&& event = static_cast<const MessagesUpdateEvent&>(e);
		auto&& client_num = event.GetClientNum();
		auto&& msg = event.GetMsg();
		printf("-----RECV-----\n");
		printf("receive from client %zd:\n", client_num);
		switch (msg.GetFormat())
		{
		case msgPack:
		{
			auto&& msg_pack = static_cast<const IMessagePack&>(msg);
			for (auto&& imsg : msg_pack.GetMsgs())
			{
				auto&& author_msg = static_cast<const AuthorizedMessage&>(*imsg);
				printf("%s : %s %s\n", author_msg.GetUsername().c_str(), toString(author_msg.GetFormat()).c_str(), author_msg.GetMsg().c_str());
			}
			break;
		}
		case getReq:
		{
			auto&& author_msg = static_cast<const AuthorizedMessage&>(msg);
			printf("%s : %s %s\n", author_msg.GetUsername().c_str(), toString(author_msg.GetFormat()).c_str(), author_msg.GetMsg().c_str());
			printf("--------------\n\n");
			break;
		}
		case file:
		case text:
		{
			auto&& author_msg = static_cast<const AuthorizedMessage&>(msg);
			printf("%s : %s %s\n", author_msg.GetUsername().c_str(), toString(author_msg.GetFormat()).c_str(), author_msg.GetMsg().c_str());
			printf("--------------\n\n");
			break;
		}
		default:
		{
			assert(0);
			return;
		}
		}		
		handleMessage(&msg, *(_clients[client_num]));
		break;
	}
	default:
	{
		assert(0);
		break;
	}
	}
}
