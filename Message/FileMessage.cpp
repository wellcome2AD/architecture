#include "FileMessage.h"
#include "../Reader/Reader.h"
#include "../Writer/Writer.h"

FileMessage::FileMessage(const std::string& username, const std::string& password, const std::string& extension, const std::string& message)
	:
	  AuthorizedMessage(username, password, message),
	  _extension(extension)
{}

void FileMessage::Read(Reader* r)
{
	*r >> _username >> _password >> _extension >> _message;
}

void FileMessage::Write(Writer* w) const
{

	*w << _username << _password << _extension << _message << my_endl();
}

format FileMessage::GetFormat() const { return file; }

std::string FileMessage::GetExtension() const { return _extension; }

void FileMessage::SetExtension(const std::string& e) { _extension = e; }
