#include <iostream>

#include "AppViewer.h"

int main()
{
	std::cout << "Welcome to app Viewer!\n";
	std::cout << std::string(10, '-') << std::endl;
	std::string ip("127.0.0.1:");
	std::cout << "Input port to connect: " << ip;
	std::string port;
	std::cin >> port;
	std::cout << "MESSAGES:\n";
	Viewer v(ip + port);
	while (true); // TODO: перенести вечный цикл в метод класса вроде run
	return 0;
}