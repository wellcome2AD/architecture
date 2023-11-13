#include <iostream>

#include "AppViewer.h"

int main()
{
	std::cout << "Welcome to app Viewer!\n";
	std::cout << std::string(10, '-') << std::endl;
	std::string url;
	std::cout << "Input URL to connect: ";
	std::cin >> url;
	Viewer v(url);
	std::cout << "MESSAGES:\n";
	while (true);
	return 0;
}