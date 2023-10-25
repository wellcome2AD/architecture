#include "TextMessage.h"
#include "../Reader/Reader.h"

void TextMessage::Read(Reader* r)
{
	*r >> _username;
	*r >> _password;
	*r >> _message;
}

format TextMessage::GetFormat() const
{
	return text;
}
