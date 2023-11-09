#include <iostream>
#include "AppViewer.h"

int main()
{
	Viewer v;
	std::cout << "Welcome to app Viewer!\n";
	auto &msgs = v.GetMsgs();
	for (auto &msg : msgs)
	{
		std::cout << msg.GetMsg();
	}
	return 0;
}