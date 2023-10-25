#include "TextMessage.h"
#include "../Reader/Reader.h"
#include "../Writer/Writer.h"

void TextMessage::Write(Writer* w) const
{
	*w << _username << _password << _message << my_endl();
}

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
