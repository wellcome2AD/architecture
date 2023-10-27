#pragma once

#include "IMessage.h"

class RequestMessage : public IMessage {
public:
	RequestMessage() = default;
	RequestMessage(const std::string& message);

	virtual format GetFormat() const override;
	virtual std::string GetMsg() const override;
	virtual void SetMsg(const std::string&) override;
	virtual void Deserialise(Deserialiser& r) override;
	virtual void Serialise(Serialiser& w) const override;
private:
	std::string _message;
};