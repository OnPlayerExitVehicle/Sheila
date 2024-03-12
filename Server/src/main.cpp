#include <iostream>
#include "hpserver.h"

using namespace networking;
using namespace hole_punching;

int main()
{
    std::cout << "Starting Server" << std::endl;

	hpserver server(3390);
	server.Start().join();
	//(void)scanf_s("%s");
	std::cerr << "You made a starvation mistake!" << std::endl;

	return 0;

	////server srv(3390);
	////srv.Start().join();
	////scanf_s("%s");
	////return 0;

	//std::cout << "Server started!" << std::endl;
	//constexpr int buffer_length = 512;
	//uint8_t receive_buffer[buffer_length];
	//int buffer_size_iterator = 0;
	//tsqueue<message> queue;
	//message msg;
	//
	//asio::io_context context;
	//udp::socket socket(context, udp::endpoint(udp::v4(), 3390));
	//udp::endpoint other;
	////socket.async_receive_from(asio::buffer(receive_buffer, buffer_length), other,
	////	[&receive_buffer, &buffer_size_iterator, &other, &socket](const std::error_code& ec, std::size_t bytes_transferred)
	////	{
	////		if(ec)
	////		{
	////			std::cout << "Error while receiving = " << ec.message() << std::endl;
	////		}
	////		else
	////		{
	////			//buffer_size_iterator += bytes_transferred / sizeof(char);
	////			//for(int i = 0; i < buffer_size_iterator; i++)
	////			//{
	////			//	//std::cout << std::format("[{}] {}", i, receive_buffer[i]) << std::endl;
	////			//	std::cout << receive_buffer[i];
	////			//}

	////			//socket.async_send_to(asio::buffer(receive_buffer, buffer_size_iterator), other, [](const std::error_code& ec, std::size_t bytes_transferred){ });
	////			message msg(receive_buffer, bytes_transferred);
	////			std::string a, b;
	////			int c, d;
	////			msg >> d >> c >> b >> a;
	////			std::cout << "a = " << a << " b = " << b << " c = " << c << " d = " << d << std::endl;
	////		}
	////	}
	////);

	//socket.async_receive_from(asio::buffer(msg.header_data(), msg.header_size()), other, 
	//	[&](const std::error_code& ec, std::size_t bytes_transferred)
	//	{
	//		if(ec)
	//		{
	//			std::cout << "Error while reading header = " << ec.message() << std::endl;
	//		}
	//		else
	//		{
	//			msg.resize_with_header();
	//			socket.async_receive_from(asio::buffer(msg.data(), msg.size()), other,
	//				[&](const std::error_code& ec, std::size_t bytes_transferred)
	//				{
	//					if(ec)
	//					{
	//						std::cout << "Error while reading body = " << ec.message() << std::endl;
	//					}
	//					else
	//					{
	//						std::string a, b;
	//						int c, d;
	//						msg >> d >> c >> b >> a;
	//						std::cout << "a = " << a << " b = " << b << " c = " << c << " d = " << d << std::endl;
	//						queue.Enqueue(msg);
	//						msg.clear();
	//					}
	//				}
	//			);
	//		}
	//	}
	//);

	//std::thread thread([&context](){ context.run(); });
	//scanf_s("%s");
	//thread.join();
}