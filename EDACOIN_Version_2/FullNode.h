#pragma once
#include "Node.h"
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

class FullNode : public Node
{
public:
	FullNode();
	~FullNode();
	void peer2peerNetFSM();
private:

	//Server side
	boost::asio::io_service* IO_Handler;
	boost::asio::ip::tcp::socket* serverSocket;
	boost::asio::ip::tcp::acceptor* serverAcceptor;
};