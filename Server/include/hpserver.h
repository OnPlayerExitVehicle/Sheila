#pragma once
#include "networking.h"
#include <algorithm>

#include "receiver.h"
#include "sender.h"
#include <map>

namespace networking::hole_punching
{
	class hpserver : protected sender, protected receiver
	{
	public:
		hpserver(short port);

		std::thread Start();

	private:
		void OnMessage(message&& msg, udp::endpoint&& sender) override;

		void SendAddressToAllClients(const udp::endpoint& address);
		void SendAllAddressesToClient(const udp::endpoint& receiver);

		asio::io_context context;
		udp::socket socket;

//		std::vector<udp::endpoint> endpoints;
        std::unordered_map<udp::endpoint, uint16_t> endpoint_peer_map;
        static uint16_t peer_id_counter;
	};
}
