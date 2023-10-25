#pragma once

#include <vector>
#include <string>
#include <map>
#include <set>
#include <memory>

#include "helpers/SocketServer.h"
#include "../Message/AuthorizedMessage.h"
#include "../Message/RequestMessage.h"

// Server that accepts connections from Clients and Viewers.
// Clients can upload text and images to Server.
class Server
{
public:
    Server() = default;
    virtual ~Server() = default;
    bool init(int port = 0); // initialize server socket, load configuration
    void run(); // accept incoming connections, receive their data and send responses

private:
    void synchState(); // this method read unread data from file resources/STATE
    void loadUsers();
    void loadRights();
    int userExists(const std::string& userName, const std::string& password) const;
    bool checkRights(const std::string& userName, format msgType) const;
    void handleMessage(IMessage* m);
    void handleAuthorizedMessage(AuthorizedMessage* m);
    std::string handleRequest(RequestMessage* tokens);

private:
    SocketServer m_socket; // server socket
    time_t last_synch_time = 0;
    long offset = 0;
    std::multimap<std::string, std::string> m_data; // representation of uploaded data
    std::map<std::string, std::string> users;
    std::map<std::string, std::vector<std::string>> rights;
};
