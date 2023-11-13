#include <time.h>
#include <algorithm>

#include "AppClient.h"
#include "../Serializer/SocketSerializer.h"
#include "../Serializer/SerializerOperators.h"
#include "../Deserializer/SocketDeserializer.h"
#include "../Deserializer/DeserializerOperators.h"
#include "../helpers/UtilString.h"
#include "../helpers/UtilFile.h"
#include "Event.h"
#include "IObserver.h"
#include "MessagesUpdateEvent.h"
#include "../Message/IMessagePack.h"

bool Client::connect(std::string url)
{
    if (_s)
    {
        return true;
    }
    _s = std::make_unique<SocketClient>();
    static constexpr int timeout = 60;
    auto start = clock();
    auto lasted = 0;
    bool succes_connect = false;
    _s->init();
    while (!(succes_connect = _s->connect(url)) && lasted < timeout)
    {
        lasted = (clock() - start) / CLOCKS_PER_SEC;
        printf("%d sec : ", lasted);
        _s->close();
        _s->init();
    }
    if (!succes_connect)
    {
        printf("connection error to %s due to timeout (%d sec)\n", url.c_str(), timeout);        
    }
    return succes_connect;
}

void Client::disconnect()
{
    if (_s)
    {
        _s.reset();
    }
}

bool Client::send(const std::string& url, const AuthorizedMessage* msg)
{    
    if (!connect(url))
    {
        return false;
    }

    SocketSerializer Serializer(_s.get());
    Serializer << msg;
    printf("data format %s, ", toString(msg->GetFormat()).c_str());
    printf("data size %zd\n", msg->GetMsg().size());    

    return true;
}

void Client::recv()
{
    if (!_s)
    {
        printf("Socket disconnected\n");
        return;
    }

    SocketDeserializer r(_s.get());
    std::shared_ptr<IMessage> msg = nullptr;
    r >> msg;
    if (msg)
    {
        auto&& msg_ptr = dynamic_cast<IMessagePack*>(msg.get());
        auto&& msg_pack_ptr = std::shared_ptr<IMessagePack>(msg_ptr);
        auto&& msg_pack = msg_pack_ptr.get()->GetMsgs();
        for (auto&& msg : msg_pack)
        {
            auto&& text_msg = dynamic_cast<TextMessage*>(msg.get());
            printf("%s : %s\n", text_msg->GetUsername().c_str(), text_msg->GetMsg().c_str());
        }
        // Notify(std::shared_ptr<Event>(new MessagesUpdateEvent(msg_pack_ptr)));
    }
}

void Client::AddObserver(std::shared_ptr<IObserver> o)
{
    _observers.push_back(o);
}

void Client::RemoveObserver(std::shared_ptr<IObserver> o)
{
    for (auto&& observer_it = _observers.begin(); observer_it != _observers.end(); ++observer_it)
    {
        if (observer_it->get() == o.get())
        {
            _observers.erase(observer_it);
        }
    }
}

void Client::Notify(std::shared_ptr<Event> e)
{
    for (auto&& o : _observers)
    {
        o->Update(e);
    }
}
