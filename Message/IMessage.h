#pragma once

#include <string>

#define SIZE_OF_FORMAT 4

class Deserializer;
class Serializer;

enum format {
	text, file, getReq
};

inline std::string toString(format f) {
	switch (f)
	{
	case text:
		return "text";
	case file:
		return "file";
	case getReq:
		return "GET";
	}
}

inline format fromString(std::string f)
{
	if (f == "text")
		return text;
	else if (f == "file")
		return file;
	else
		return getReq;
}

class IMessage {
public:
	virtual format GetFormat() const = 0;
	virtual std::string GetMsg() const = 0;
	virtual void SetMsg(const std::string& m) = 0;
	virtual void Deserialize(Deserializer& r) = 0;
	virtual void Serialize(Serializer& w) const = 0;
};
