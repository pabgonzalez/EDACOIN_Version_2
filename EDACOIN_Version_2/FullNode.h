#pragma once
#include "Node.h"
#include "ServerRequest.h"
#include "ServerResponse.h"
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

typedef enum { IDLE, WAITING_LAYOUT, COLLECTING_NETWORK_MEMBERS, NETWORK_CREATED } p2pState;

class FullNode : public Node
{
public:
	FullNode();
	~FullNode();
	void p2pNetFSM();

	void acceptConnection();
	void writeResponse();
	void readRequest();
private:
	bool pingStatus;
	unsigned timer;
	p2pState state;

	//Server side
	char buf[512];
	bool readingRequest;
	bool writingResponse;
	Request request;
	Response response;
	boost::asio::io_service* IO_Handler;
	boost::asio::ip::tcp::socket* serverSocket;
	boost::asio::ip::tcp::acceptor* serverAcceptor;
	void readHandler(const boost::system::error_code& error, std::size_t len);
	void writeHandler(const boost::system::error_code& error, std::size_t len);
};