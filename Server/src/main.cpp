#include <iostream>
#include "hpserver.h"

using namespace networking;
using namespace hole_punching;

int main()
{
    std::cout << "Starting Server" << std::endl;

	hpserver server(3390);
	server.Start().join();
	std::cerr << "You made a starvation mistake!" << std::endl;

	return 0;
}