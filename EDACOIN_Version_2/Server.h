#pragma once
#include <iostream>
#include <cstdio>
#include <cstring>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "ServerResponse.h"
#include "ServerRequest.h"

using namespace std;

class Server
{
public:
	Server(unsigned int port);
	~Server();
	bool acceptConnection();
	bool writeResponse();
	bool readRequest();

private:
	//Server side
	string receivedMessage;
	bool readingRequest;
	bool writingResponse;
	Request request;
	Response response;
	boost::asio::io_service* IO_Handler;
	boost::asio::ip::tcp::socket* serverSocket;
	boost::asio::ip::tcp::acceptor* serverAcceptor;
};