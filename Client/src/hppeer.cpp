#include "hppeer.h"

namespace networking::hole_punching
{
	hppeer::hppeer(uint16_t id, asio::io_context& context, udp::socket& socket, udp::endpoint&& endpoint) : sender(context, socket), /*context(context), socket(socket),*/ endpoint{ endpoint }, id{ id } { }

	void hppeer::StartHolePunching()
	{
		SendHolePunching();
	}

	void hppeer::SendHolePunching()
	{
		std::cout << "SendHolePunching" << std::endl;
		std::shared_ptr<message> msg = std::make_shared<message>();
		msg->header.flag = hpflag::CONNECTION_TO_PEER_REQUEST;
		std::cout << this << std::endl;

		SendInternalAfter(msg, endpoint, 250,
			[this]()
			{
				if (!hole_punching_received)
					SendHolePunching();
				else
					SendHolePunchingReceived();
			}
		);
	}

	void hppeer::SendHolePunchingReceived()
	{
		std::shared_ptr<message> msg = std::make_shared<message>();
		msg->header.flag = hpflag::CONNECTION_TO_PEER_REQUEST_RECEIVED;

		SendInternalAfter(msg, endpoint, 250,
			[this]()
			{
				if (!hole_punched)
					SendHolePunchingReceived();
			}
		);
	}

	void hppeer::OnMessage(message&& msg)
	{
		std::string text;

		switch (msg.header.flag)
		{
		case CONNECTION_TO_PEER_REQUEST:
			hole_punching_received = true;
			break;

		case CONNECTION_TO_PEER_REQUEST_RECEIVED:
			if(!hole_punched)
			{
				hole_punched = true;
				std::cout << "Hole punching success with client on address" << endpoint << std::endl;
				if (!any_hole_punched)
				{
					any_hole_punched = true;
					if (on_any_hole_punched)
						on_any_hole_punched();
				}
			}
			
			break;

		case MESSAGE:
			msg >> text;
			std::cout << std::endl << "Message from other client = " << text << std::endl;

			if(on_message_received)
				on_message_received();
			break;

		default:
			std::cout << "Error" << std::endl;
			break;
		}
	}

	//tsqueue<message>& hppeer::GetMessageQueue()
	//{
	//	return message_queue;
	//}

	void hppeer::Send(std::shared_ptr<message> msg)
	{
		if (!hole_punched)
		{
			std::cout << "[!] NAT hole punch is not achieved with given client yet" << std::endl;
			return;
		}

		SendInternal(msg, endpoint);
	}
}