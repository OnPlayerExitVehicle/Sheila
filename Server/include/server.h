//#pragma once
//#include "networking.h"
//
//class server
//{
//public:
//	server(short port) : socket(context, udp::endpoint(udp::v4(), port)) { }
//
//	std::thread Start()
//	{
//		WaitForConnection();
//		return std::thread([this](){ context.run(); });
//	}
//
//private:
//
//	void WaitForConnection()
//	{
//		socket.async_receive_from(asio::buffer(_last_msg.header_data(), _last_msg.header_size()), _last_endpoint,
//			[this](const std::error_code& ec, std::size_t b1)
//			{
//				if (ec)
//					std::cerr << ec.message() << std::endl;
//				else
//				{
//					_last_msg.resize_with_header();
//
//					_sockets.push_back(std::move(udp::socket(context)));
//					udp::socket& last = _sockets.back();
//					last.async_connect(_last_endpoint,
//						[&](const std::error_code& ec)
//						{
//							last.async_receive(asio::buffer(_last_msg.data(), _last_msg.size()),
//								[&](const std::error_code& ec, size_t b2)
//								{
//									if (ec)
//										std::cerr << ec.message() << std::endl;
//									else
//									{
//										std::cout << std::format("{} bytes transferred from endpoint = {}", b2, _last_endpoint.address().to_string());
//									}
//								}
//							);
//
//						}
//					);
//
//					WaitForConnection();
//				}
//			}
//		);
//	}
//
//	asio::io_context context;
//	udp::socket socket;
//
//	std::vector<udp::socket> _sockets;
//
//	udp::endpoint _last_endpoint;
//	networking::message _last_msg;
//};