#include <time.h>

#include "AppClient.h"
#include "helpers/SocketClient.h"
#include "../Serializer/SocketSerializer.h"
#include "../Serializer/SerializerOperators.h"
#include "../helpers/UtilString.h"
#include "../helpers/UtilFile.h"

bool Client::send(const std::string& url, const AuthorizedMessage* msg)
{
    SocketClient s;
    static constexpr int timeout = 60;
    auto start = clock();
    int len = 0;

    bool succes_connect = false;
    auto lasted = 0;
    s.init();
    while (!(succes_connect = s.connect(url)) && lasted < timeout)
    {
        lasted = (clock() - start) / CLOCKS_PER_SEC;
        printf("%d sec : ", lasted);
        s.close();
        s.init();
    }

    if (succes_connect)
    {        
        SocketSerializer Serializer(&s);
        Serializer << msg;
        printf("data format %s, ", toString(msg->GetFormat()).c_str());
        printf("data size %zd\n", msg->GetMsg().size());
    }
    else
    {
        printf("connection error to %s due to timeout (%d sec)\n", url.c_str(), timeout);
    }

    return len > 0;
}
