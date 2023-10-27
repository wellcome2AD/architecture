#pragma once

class Deserialiser
{
public:
	virtual void Recv() = 0;
	virtual char GetChar() = 0;
};
