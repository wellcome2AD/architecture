#include "FileMessage.h"
#include "../Reader/Reader.h"

FileMessage::FileMessage(const std::string& username, const std::string& password, const std::string& extension, const std::string& message)
	:
	  AuthorizedMessage(username, password, message),
	  _extension(extension)
{}

void FileMessage::Read(Reader* r)
{
	*r >> _username;
	*r >> _password;
	*r >> _extension;
	*r >> _message;
}

format FileMessage::GetFormat() const { return file; }

std::string FileMessage::GetExtension() const { return _extension; }

void FileMessage::SetExtension(const std::string& e) { _extension = e; }
