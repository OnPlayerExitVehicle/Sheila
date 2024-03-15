#pragma once
#include "networking.h"

namespace networking
{
	class receiver
	{
	public:
		receiver(asio::io_context& context, udp::socket& socket) : context(context), socket(socket) { }

	protected:
		void Listen()
		{
			//std::cout << "[Receiver::Listen]" << std::endl;
			socket.async_receive_from(asio::buffer(data, 256), tmp_endpoint,
				[this](const std::error_code& ec, size_t bytes_transferred)
				{
					if(ec)
					{
						std::cerr << "[Receiver] Error = " << ec.message() << std::endl;
						return;
					}

					//std::shared_ptr<message> msg = std::make_shared<message>(data, bytes_transferred);
					message msg(data, bytes_transferred);
					//msg.unpack();
					OnMessage(std::move(msg), std::move(tmp_endpoint));
				}
			);
		}

		virtual void OnMessage(message&& msg, udp::endpoint&& sender) = 0;

	private:
		asio::io_context& context;
		udp::socket& socket;

		uint8_t data[256];
		udp::endpoint tmp_endpoint;
	};
}