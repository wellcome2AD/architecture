#pragma once

#include "IMessage.h"

class AuthorizedMessage : public IMessage {
public:
	AuthorizedMessage() = default;
	AuthorizedMessage(const std::string& username, const std::string& password, const std::string& message);

	virtual format GetFormat() const = 0;
	virtual std::string GetMsg() const override;
	virtual void SetMsg(const std::string& m) override;
	virtual void Deserialize(Deserializer& d) = 0;
	virtual void Serialize(Serializer& s) const = 0;

	virtual std::string GetUsername() const;
	virtual void SetUsername(const std::string& n);
	virtual std::string GetPassword() const;
	virtual void SetPassword(const std::string& p);

protected:
	std::string _username, _password, _message;
};
