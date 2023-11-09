#pragma once

#include <vector>
#include <memory>

#include "IMessage.h"

class IMessagePack : public IMessage
{
	virtual void AddMsg(std::shared_ptr<IMessage> msg)    = 0;
	virtual void RemoveMsg(std::shared_ptr<IMessage> msg) = 0;
	virtual std::vector<std::shared_ptr<IMessage>> GetMsgs() const     = 0;
};
