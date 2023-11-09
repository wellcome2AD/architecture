#pragma once

class Serializer
{
public:
	virtual void PutChar(char c) = 0;
	virtual void Flush() = 0;
};
