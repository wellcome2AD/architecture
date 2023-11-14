#include "MessagePack.h"
#include "../Deserializer/DeserializerOperators.h"
#include "../Serializer/SerializerOperators.h"

MessagePack::MessagePack(const std::vector<std::shared_ptr<IMessage>>& msgs)
    : _msgs(msgs) 
{}

format MessagePack::GetFormat() const { return msgPack; }

std::string MessagePack::GetMsg() const { return std::string(); }

void MessagePack::SetMsg(const std::string& m) {}

void MessagePack::Deserialize(Deserializer& d)
{
    d >> _msgs;
}

void MessagePack::Serialize(Serializer& s) const
{
    s << _msgs << my_endl();
}

void MessagePack::AddMsg(std::shared_ptr<IMessage> msg)
{
    _msgs.push_back(msg);
}

#include <algorithm>

void MessagePack::RemoveMsg(std::shared_ptr<IMessage> msg)
{
    for (auto m = _msgs.begin(); m != _msgs.end(); ++m)
    {
        if ((*m)->GetFormat() == msg->GetFormat() && (*m)->GetMsg() == msg->GetMsg())
        {
            _msgs.erase(m);
        }
    }
}

std::vector<std::shared_ptr<IMessage>> MessagePack::GetMsgs() const
{
    return _msgs;
}
