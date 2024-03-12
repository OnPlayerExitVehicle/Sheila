#include "hpserver.h"

namespace networking::hole_punching
{
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
			if (std::find(endpoints.begin(), endpoints.end(), sender) == endpoints.end())
			{
				endpoints.push_back(sender);
				if (endpoints.size() > 1)
				{
					std::cout << "Total connection count is " << endpoints.size() << std::endl;
					SendAddressToAllClients(sender); // Send address of this client to all
					SendAllAddressesToClient(sender); // Send address of all clients to this
				}
			}

			break;

		default:
			std::cout << "Uncaught request came from client = " << sender << ", flag = " << (int)flag << std::endl;
			break;
		}

		Listen();
	}

	//void hpserver::Listen()
	//{
	//	std::cout << "Server::Listen" << std::endl;

	//	socket.async_receive_from(asio::buffer(tmp_incoming_msg.header_data(), tmp_incoming_msg.header_size()), tmp_endpoint,
	//		[this](const std::error_code& ec, size_t bytes_transferred)
	//		{
	//			if (ec)
	//			{
	//				std::cerr << "[Server::Listen (Header)] Error = " << ec.message() << std::endl;
	//				return;
	//			}

	//			std::cout << "Received first half, bytes = " << bytes_transferred << std::endl;


	//			tmp_incoming_msg.resize_with_header();
	//			
	//			asio::async_read(socket, asio::buffer(tmp_incoming_msg.data(), tmp_incoming_msg.size()), tmp_endpoint,
	//				[this](const std::error_code& ec, size_t bytes_transferred)
	//				{
	//					if (ec)
	//					{
	//						std::cerr << "[Server::Listen (Body)] Error = " << ec.message() << std::endl;
	//						return;
	//					}

	//					std::cout << "Received second half, bytes = " << bytes_transferred << std::endl;

	//					switch (tmp_incoming_msg.header.flag)
	//					{
	//					case CONNECTION_TO_MAIN_SERVER_REQUEST:
	//						std::cout << "Connection request came from client = " << tmp_endpoint << std::endl;
	//						if (std::find(endpoints.begin(), endpoints.end(), tmp_endpoint) == endpoints.end())
	//						{
	//							endpoints.push_back(tmp_endpoint);
	//							if (endpoints.size() > 1)
	//							{
	//								std::cout << "Total connection count is " << endpoints.size() << std::endl;
	//								SendAddressToAllClients(tmp_endpoint); // Send address of this client to all
	//								SendAllAddressesToClient(tmp_endpoint); // Send address of all clients to this
	//								Listen();
	//							}
	//							else
	//							{
	//								Listen();
	//							}
	//						}

	//						break;

	//					default:
	//						std::cout << "Uncaught request came from client = " << tmp_endpoint << ", flag = " << (int)tmp_incoming_msg.header.flag << std::endl;
	//						Listen();
	//					}


	//				}
	//			);
	//		}
	//	);
	//}

	void hpserver::SendAddressToAllClients(const udp::endpoint& address)
	{
		std::shared_ptr<message> msg = std::make_shared<message>();
		msg->header.flag = hpflag::OTHER_CLIENT_ENDPOINT;
		*msg << address;

		for(int i = 0; i < endpoints.size(); i++)
		{
			if(endpoints[i] == address) continue;

			SendInternal(msg, endpoints[i]);
		}
	}

	void hpserver::SendAllAddressesToClient(const udp::endpoint& receiver)
	{
		std::shared_ptr<message> msg = std::make_shared<message>();
		msg->header.flag = hpflag::OTHER_CLIENT_ENDPOINT;

		udp::endpoint placeholder;

		for (int i = 0; i < endpoints.size(); i++)
		{
			if (endpoints[i] == receiver) continue;

			*msg << endpoints[i];
			SendInternal(msg, receiver);
			*msg >> placeholder;
		}
	}
}