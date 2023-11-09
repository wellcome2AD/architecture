#pragma once

#include <vector>

#include "../Client/AppClient.h"
#include "../Message/IMessage.h"

class Viewer
{
public:
	Viewer() = default;
	const std::vector<IMessage*>& GetMsgs() const;

private:
	void Notify(std::vector<IMessage*> msgs); // to do написать компоновщик типа AuthorizedMessagesPack	

private:
	friend Client;
	Client _client;
	std::vector<IMessage*> _msgs;
};

