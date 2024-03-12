#pragma once
#include "networking.h"

namespace networking
{
	class sender
	{
	public:
		sender(asio::io_context& context, udp::socket& socket) : context(context), socket(socket) { }

	protected:
		void SendInternal(std::shared_ptr<message> msg, const udp::endpoint& endpoint) const;
		void SendInternal(std::shared_ptr<message> msg, const udp::endpoint& endpoint, std::function<void()> success) const;
		void SendInternalAfter(std::shared_ptr<message> msg, const udp::endpoint& endpoint, const long long milliseconds) const;
		void SendInternalAfter(std::shared_ptr<message> msg, const udp::endpoint& endpoint, const long long milliseconds, std::function<void()> success) const;

	private:
		asio::io_context& context;
		udp::socket& socket;
	};

	inline void sender::SendInternal(std::shared_ptr<message> msg, const udp::endpoint& endpoint) const
	{
		msg->pack();

		socket.async_send_to(asio::buffer(msg->buffer()), endpoint,
			[&endpoint, this](const std::error_code& ec, size_t bytes_transferred)
			{
				if (ec)
				{
					std::cerr << "[SendInternal] Error = " << ec.message() << std::endl;
					return;
				}
			}
		);
	}

	inline void sender::SendInternal(std::shared_ptr<message> msg, const udp::endpoint& endpoint, std::function<void()> success) const
	{
		msg->pack();

		socket.async_send_to(asio::buffer(msg->buffer()), endpoint,
			[success, this](const std::error_code& ec, size_t bytes_transferred)
			{
				if (ec)
				{
					std::cerr << "[SendInternal (Header)] Error = " << ec.message() << std::endl;
					return;
				}

				success();
			}
		);
	}

	inline void sender::SendInternalAfter(std::shared_ptr<message> msg, const udp::endpoint& endpoint, const long long milliseconds) const
	{
		msg->pack();

		std::shared_ptr<asio::steady_timer> ptr = std::make_shared<asio::steady_timer>(context, std::chrono::milliseconds(milliseconds));
		ptr->async_wait(
			[&msg, &endpoint, ptr, this](const std::error_code& ec)
			{
				if (ec)
				{
					std::cerr << "[Async Wait] Error = " << ec.message() << std::endl;
					return;
				}
				socket.async_send_to(asio::buffer(msg->buffer()), endpoint,
				[&endpoint, this](const std::error_code& ec, size_t bytes_transferred)
					{
						if (ec)
						{
							std::cerr << "[SendInternalAfter (Header)] Error = " << ec.message() << std::endl;
							return;
						}
					}
				);
			}
		);
	}

	inline void sender::SendInternalAfter(std::shared_ptr<message> msg, const udp::endpoint& endpoint, const long long milliseconds, std::function<void()> success) const
	{
		msg->pack();

		std::shared_ptr<asio::steady_timer> ptr = std::make_shared<asio::steady_timer>(context, std::chrono::milliseconds(milliseconds));
		
		ptr->async_wait(
			[&msg, success, &endpoint, ptr, this](const std::error_code& ec)
			{
				if (ec)
				{
					std::cerr << "[Async Wait] Error = " << ec.message() << std::endl;
					return;
				}
				socket.async_send_to(asio::buffer(msg->buffer()), endpoint,
					[success, &endpoint, this](const std::error_code& ec, size_t bytes_transferred)
					{
						if (ec)
						{
							std::cerr << "[SendInternal (Header)] Error = " << ec.message() << std::endl;
							return;
						}
						success();
					}
				);
			}
		);
	}
}
