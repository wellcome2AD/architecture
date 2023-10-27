#include "TextMessage.h"
#include "../Deserialiser/DeserialiserOperators.h"
#include "../Serialiser/SerialiserOperators.h"

void TextMessage::Serialise(Serialiser& s) const
{
	s << _username << _password << _message << my_endl();
}

void TextMessage::Deserialise(Deserialiser& d)
{
	d >> _username >> _password >> _message;
}

format TextMessage::GetFormat() const
{
	return text;
}
