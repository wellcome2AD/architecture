#pragma once

#include "AuthorizedMessage.h"

class FileMessage : public AuthorizedMessage {
public:
	FileMessage() = default;
	FileMessage(const std::string& username, const std::string& password, const std::string& extension, const std::string& message);
	
	virtual void Read(Reader*) override;
	virtual format GetFormat() const override;

	std::string GetExtension() const;
	void SetExtension(const std::string& e);

private:
	std::string _extension;
};

inline std::ostream& operator<<(std::ostream& os, const FileMessage& m) {
	os << m.GetUsername() << m.GetPassword() << toString(m.GetFormat()) << m.GetMsg();
	return os;
}
