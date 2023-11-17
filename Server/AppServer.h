#pragma once

#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <map>

#include "helpers/SocketServer.h"
#include "ResponseBuilder/ServerContent.h"
#include "../Message/AuthorizedMessage.h"
#include "../Message/RequestMessage.h"
#include "ClientConnection/ClientConnection.h"
#include "../Observer/IObserver.h"
#include "../Message/MessagePack.h"

// Server that accepts connections from Clients and Viewers.
// Clients can upload text and images to Server.
class Server : public IObserver
{
public:
    Server() = default;
    virtual ~Server();
    bool init(int port = 0); // initialize server socket, load configuration
    void run(); // accept incoming connections, receive their data and send responses

private:
    void synchState(); // this method read unread data from file resources/STATE
    void loadUsers();
    void loadRights();
    int userExists(const std::string& userName, const std::string& password) const;
    bool checkRights(const std::string& userName, format msgType) const;
    void handleMessage(const IMessage* m, ClientConnection& client);
    void handleAuthorizedMessage(const AuthorizedMessage* m);
    std::string handleRequest(const RequestMessage* tokens);
    void broadcast(const IMessagePack& msg_pack);
    MessagePack convertSContToMsgPack() const;

    // Унаследовано через IObserver
    virtual void Update(const Event& e) override;

private:
    SocketServer m_socket; // server socket
    
    std::vector<std::shared_ptr<ClientConnection>> _clients;
    std::mutex _clients_mutex;

    time_t last_synch_time = 0;
    long offset = 0;
    std::unique_ptr<ServerContent> m_data = std::make_unique<ServerContent>();
    std::mutex m_data_mutex;
    std::map<std::string, std::string> users;
    std::map<std::string, std::vector<std::string>> rights;

    friend class ClientConnection;
};
