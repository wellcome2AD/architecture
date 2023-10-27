#pragma once

class Deserializer
{
public:
	virtual void Recv() = 0;
	virtual char GetChar() = 0;
};
