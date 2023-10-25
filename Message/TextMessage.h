#pragma once

#include "AuthorizedMessage.h"

class TextMessage : public AuthorizedMessage {
public:
	using AuthorizedMessage::AuthorizedMessage;
	virtual void Read(Reader*) override;
	virtual format GetFormat() const override;
};

inline std::ostream& operator<<(std::ostream& os, const TextMessage& m) {
	os << m.GetUsername() << m.GetPassword() << toString(m.GetFormat()) << m.GetMsg();
	return os;
}
