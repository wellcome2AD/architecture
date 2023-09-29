#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <WS2tcpip.h>

#include "SocketClient.h"
#include "UtilString.h"

#include <ctime>

bool SocketClient::connect(const std::string& url)
{
    struct sockaddr_in server;
    const std::vector<std::string>& urlTokens = split(url, ":");
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(urlTokens[0].c_str());
    server.sin_port = htons(toInt(urlTokens[1]));

    int successful_connection = -1;
    auto start = clock();
    static constexpr int timeout = 60;
    while ((successful_connection == -1) && ((clock() - start) / CLOCKS_PER_SEC < timeout))
    {
        successful_connection = ::connect(m_socket, (struct sockaddr*)&server, sizeof(server)); 
    }
    if (successful_connection == -1)
    {
        printf("connection error to %s due timeout (%d sec)\n", url.c_str(), timeout);
        close();
        m_socket = -1;
        return false;
    }
    return true;
}
