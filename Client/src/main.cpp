#include <iostream>

#include "hpclient.h"
#include "networking.h"

using namespace networking;
using namespace hole_punching;

void WaitForInput();
hpclient* client;
bool br = false;
bool b = false;

int main()
{
	srand(time(NULL));
	std::string ip;
	std::cout << "IP = ";
	std::getline(std::cin, ip);

	client = new hpclient(rand() % 65536);
	std::thread client_thread = client->Start(ip, 3390);

	std::cout << "Client started!" << std::endl;

//	hppeer::on_any_hole_punched = []()
//	{
//			b = true;
//	};
//
//	hppeer::on_message_received = []()
//	{
//			br = true;
//	};

	while (!b) continue;
	WaitForInput();

	client_thread.join();
	std::cerr << "You made a starvation mistake!" << std::endl;
	return 0;
}

void WaitForInput()
{
	br = false;
	while (!br)
	{
		std::string str;
		std::cout << "You = ";
		std::getline(std::cin, str);
		/*std::cin >> str;*/
		client->SendToAll(str);
	}

	WaitForInput();
}