#pragma once

#include "IMessage.h"

class RequestMessage : public IMessage {
public:
	RequestMessage() = default;
	virtual ~RequestMessage() override = default;
	RequestMessage(const std::string& message);

	virtual format GetFormat() const override;
	virtual std::string GetMsg() const override;
	virtual void SetMsg(const std::string&) override;
	virtual void Deserialize(Deserializer& r) override;
	virtual void Serialize(Serializer& w) const override;
private:
	std::string _message;
};