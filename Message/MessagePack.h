#pragma once

#include <vector>
#include <memory>
#include "IMessagePack.h"

class MessagePack : public IMessagePack
{
public:
	MessagePack() = default;
	virtual ~MessagePack() override = default;
	MessagePack(const std::vector<std::shared_ptr<IMessage>>& msgs);
	virtual format GetFormat() const override;
	virtual void Deserialize(Deserializer& r) override;
	virtual void Serialize(Serializer& w) const override;
	virtual void AddMsg(std::shared_ptr<IMessage> msg) override;
	virtual void RemoveMsg(std::shared_ptr<IMessage> msg) override;
	virtual std::vector<std::shared_ptr<IMessage>> GetMsgs() const override;

private:
	virtual std::string GetMsg() const override;
	virtual void SetMsg(const std::string& m) override;

private:
	std::vector<std::shared_ptr<IMessage>> _msgs;
};
