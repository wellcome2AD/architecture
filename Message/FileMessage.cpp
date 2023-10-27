#include "FileMessage.h"
#include "../Serialiser/SerialiserOperators.h"
#include "../Deserialiser/DeserialiserOperators.h"

FileMessage::FileMessage(const std::string& username, const std::string& password, const std::string& extension, const std::string& message)
	:
	  AuthorizedMessage(username, password, message),
	  _extension(extension)
{}

void FileMessage::Deserialise(Deserialiser& r)
{
	r >> _username >> _password >> _extension >> _message;
}

void FileMessage::Serialise(Serialiser& w) const
{

	w << _username << _password << _extension << _message << my_endl();
}

format FileMessage::GetFormat() const { return file; }

std::string FileMessage::GetExtension() const { return _extension; }

void FileMessage::SetExtension(const std::string& e) { _extension = e; }
