#include "TextMessage.h"
#include "../Deserializer/DeserializerOperators.h"
#include "../Serializer/SerializerOperators.h"

void TextMessage::Serialize(Serializer& s) const
{
	s << _username << _password << _message << my_endl();
}

void TextMessage::Deserialize(Deserializer& d)
{
	d >> _username >> _password >> _message;
}

format TextMessage::GetFormat() const
{
	return text;
}
