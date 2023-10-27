#pragma once

class Serialiser
{
public:
	virtual void PutChar(char c) = 0;
	virtual void Flush() = 0;
};
