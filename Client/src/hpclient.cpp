#include "hpclient.h"

namespace networking::hole_punching
{
	hpclient::hpclient(unsigned short port) : sender(context, socket), receiver(context, socket), socket(context, udp::endpoint(udp::v4(), port)) { }

	std::thread hpclient::Start(const std::string& address, short port)
	{
		server_endpoint = udp::endpoint(asio::ip::address::from_string(address), port);

        std::shared_ptr<message> msg = std::make_shared<message>();
		msg->header.flag = hpflag::CONNECTION_TO_MAIN_SERVER_REQUEST;

		SendInternal(msg, server_endpoint);
		Listen();

		return std::thread([this] { context.run(); });
	}

	hpclient::~hpclient()
	{
		for(auto it = peer_map.begin(); it != peer_map.end(); ++it)
		{
			delete it->second;
		}
	}


	void hpclient::Send(const std::string& text, uint16_t peer)
	{
		std::shared_ptr<message> msg;
		msg->header.flag = hpflag::MESSAGE;
		*msg << text;

		if(peer_map.contains(peer))
		{
			peer_map[peer]->Send(msg);
		}
	}

	void hpclient::SendToAll(const std::string& text)
	{
		std::shared_ptr<message> msg = std::make_shared<message>();
		msg->header.flag = hpflag::MESSAGE;
		*msg << text;

		for (auto it = peer_map.begin(); it != peer_map.end(); ++it)
		{
			it->second->Send(msg);
		}
	}

	void hpclient::OnMessage(message&& msg, udp::endpoint&& sender)
	{
		udp::endpoint ep;
		uint16_t peer_id;

		switch (msg.header.flag)
		{
		case OTHER_CLIENT_ENDPOINT:
			msg >> ep;
			msg >> peer_id;

			std::cout << "OTHER_CLIENT_ENDPOINT from server = " << ep << std::endl;

			if (!peer_map.contains(peer_id))
			{
				endpoint_peer_map[ep] = peer_id;
				(peer_map[peer_id] = new hppeer(peer_id, context, socket, std::move(ep)))->StartHolePunching();
			}
			break;

		default:

			if(endpoint_peer_map.contains(sender))
			{
				peer_map[endpoint_peer_map[sender]]->OnMessage(std::move(msg));
			}
			break;
		}

		Listen();
	}
}