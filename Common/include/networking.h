#pragma once

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <thread>

typedef asio::ip::udp udp;
typedef asio::ip::tcp tcp;

#include "message.h"
#include "tsqueue.h"

enum hpflag : uint8_t
{
	CONNECTION_TO_MAIN_SERVER_REQUEST = 0,
	OTHER_CLIENT_ENDPOINT,
	OTHER_CLIENT_DISCONNECT,
	CONNECTION_TO_PEER_REQUEST,
	CONNECTION_TO_PEER_REQUEST_RECEIVED,
	MESSAGE
};

inline std::ostream& operator << (std::ostream& os, const udp::endpoint& ep)
{
	os << ep.address() << ':' << ep.port();
	return os;
}