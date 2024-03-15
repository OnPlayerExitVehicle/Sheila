#pragma once
#include "networking.h"
#include <map>
#include <format>

#include "hppeer.h"
#include "receiver.h"
#include "sender.h"

namespace networking::hole_punching
{
	class hpclient : protected sender, protected receiver
	{
	public:
		hpclient(unsigned short port);
		hpclient() = delete;
		hpclient(const hpclient&) = delete;
		hpclient(hpclient&&) = delete;
		hpclient& operator = (const hpclient&) = delete;
		hpclient& operator = (hpclient&&) = delete;
		virtual ~hpclient();

		std::thread Start(const std::string& address, short port);


		void Send(const std::string& text, uint16_t peer);
        void Send(std::shared_ptr<message> msg, uint16_t peer);
		void SendToAll(const std::string& text);
        void SendToAll(std::shared_ptr<message> msg);

        //std::vector<owned_message> messages;
        tsqueue<owned_message> message_queue;

	private:
		void OnMessage(message&& msg, udp::endpoint&& sender) override;

		asio::io_context context;
		udp::socket socket;

		std::map<uint16_t, hppeer*> peer_map;
		std::map<udp::endpoint, uint16_t> endpoint_peer_map;
		udp::endpoint server_endpoint;
	};
}
