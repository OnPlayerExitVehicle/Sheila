#include "networking.h"

class udp_connection
{
public:
	udp_connection(asio::io_context& context) : _context(context), _socket(context) { } // For client (outgoing)
	udp_connection(asio::io_context& context, udp::endpoint endpoint) : _context(context), _socket(context)
	{
		_socket.async_connect(endpoint, [this](const std::error_code& ec)
		{
				ReadHeader();
		});
	} // For server (outgoing)

	udp_connection(const udp_connection&) = delete;
	udp_connection& operator = (const udp_connection&) = delete;

	virtual ~udp_connection() = default;

	void connect_to_server(const udp::endpoint& endpoint)
	{
		asio::async_connect(_socket, endpoint,
			[this](const std::error_code& ec, udp::endpoint ep)
			{
				if(ec)
				{
					std::cerr << "Error connecting to server!" << std::endl;
				}
				else
				{
					ReadHeader();
				}
			}
		);
	}

	void connect_to_client()
	{
		ReadHeader();
	}

private:
	void ReadHeader()
	{
		_socket.async_receive_from(asio::buffer(in_msg.header_data(), in_msg.header_size()), in_endpoint,
			[this](const std::error_code& ec, size_t bytes_transferred)
			{
				if(ec)
				{
					udp::socket sckt(_context);
					std::cerr << "Error while reading header" << std::endl;
				}
				else
				{
					in_msg.resize_with_header();
					ReadBody();
				}
			}
		);
	}

	void ReadBody()
	{
		_socket.async_receive_from(asio::buffer(in_msg.data(), in_msg.size()), in_endpoint,
			[this](const std::error_code& ec, size_t bytes_transferred)
			{
				if(ec)
				{
					std::cerr << "Error while reading body" << std::endl;
				}
				else
				{
					_msg_queue.Enqueue(in_msg);
					ReadHeader();
				}
			}
		);
	}

	void WriteHeader();
	void WriteBody();

	asio::io_context& _context;
	tsqueue<message>& _msg_queue;
	udp::socket _socket;

	message in_msg;
	udp::endpoint in_endpoint;
};