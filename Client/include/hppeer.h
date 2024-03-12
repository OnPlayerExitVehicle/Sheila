#pragma once

#include "networking.h"
#include "sender.h"

namespace networking::hole_punching
{
	class hppeer : protected sender
	{
	public:
		hppeer(const hppeer&) = delete;
		hppeer& operator = (const hppeer&) = delete;

		hppeer(uint16_t id, asio::io_context& context, udp::socket& socket, udp::endpoint&& endpoint);

		void StartHolePunching();

		void Send(std::shared_ptr<message> msg);

		void OnMessage(message&& msg);

	private:

		void SendHolePunching();

		void SendHolePunchingReceived();

		udp::endpoint endpoint;
		uint16_t id;

		bool hole_punching_received = false;
		bool hole_punched = false;

	public:
//		inline static bool any_hole_punched = false;
//		inline static std::function<void()> on_any_hole_punched;
//		inline static std::function<void()> on_message_received;
	};
}

