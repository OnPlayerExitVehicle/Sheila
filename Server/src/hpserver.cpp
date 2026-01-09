#include "hpserver.h"

namespace networking::hole_punching
{
    uint16_t hpserver::peer_id_counter = 1;

	hpserver::hpserver(short port) : sender(context, socket), receiver(context, socket), socket(context, udp::endpoint(udp::v4(), port)) { }

	std::thread hpserver::Start()
	{
		Listen();
		return std::thread([this]() { context.run(); });
	}

	void hpserver::OnMessage(message&& msg, udp::endpoint&& sender)
	{
        uint8_t flag = msg.header.flag;
		switch (flag)
		{
		case CONNECTION_TO_MAIN_SERVER_REQUEST:
			std::cout << "Connection request came from client = " << sender << std::endl;
			if (!endpoint_peer_map.contains(sender))
			{
                endpoint_peer_map[sender] = peer_id_counter++;
				if (endpoint_peer_map.size() > 1)
				{
					std::cout << "Total connection count is " << endpoint_peer_map.size() << std::endl;
					SendAddressToAllClients(sender); // Send address of this client to all clients
					SendAllAddressesToClient(sender); // Send address of all clients to this client
				}
			}

			break;

		default:
			std::cout << "Uncaught request came from client = " << sender << ", flag = " << (int)flag << std::endl;
			break;
		}

		Listen();
	}

	void hpserver::SendAddressToAllClients(const udp::endpoint& address)
	{
		std::shared_ptr<message> msg = std::make_shared<message>();
		msg->header.flag = hpflag::OTHER_CLIENT_ENDPOINT;
        *msg << endpoint_peer_map[address];
		*msg << address;

        for(auto it = endpoint_peer_map.begin(); it != endpoint_peer_map.end(); it++)
        {
            if(it->first == address) continue;
            SendInternal(msg, it->first);
        }
	}

	void hpserver::SendAllAddressesToClient(const udp::endpoint& receiver)
	{
		std::shared_ptr<message> msg = std::make_shared<message>();
		msg->header.flag = hpflag::OTHER_CLIENT_ENDPOINT;

		udp::endpoint placeholder;
        uint16_t placeholder2;

        for(auto it = endpoint_peer_map.begin(); it != endpoint_peer_map.end(); it++)
        {
            if(it->first == receiver) continue;

            *msg << it->second;
            *msg << it->first;

            SendInternal(msg, receiver);

            *msg >> placeholder;
            *msg >> placeholder2;
        }
	}
}