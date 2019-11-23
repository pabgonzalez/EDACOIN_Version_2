#pragma once
#include <iostream>
#include <cstdio>
#include <cstring>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

using namespace std;

class Server
{
public:
	Server(unsigned int port);
	~Server();
	bool acceptConnection();	//Acepta la conexion y pasa a readingRequest
	bool readRequest();			//Lee el mensaje y lo guarda en receivedMessage
	bool writeResponse();		
	string getRequest();		//Devuelve receivedMessage
	void sendResponse(string status, string content);	//Envia la respuesta deseada

private:
	//Server side
	string receivedMessage;
	bool readingRequest;
	bool requestReady;
	bool writingResponse;
	string response;
	boost::asio::io_service* IO_Handler;
	boost::asio::ip::tcp::socket* serverSocket;
	boost::asio::ip::tcp::acceptor* serverAcceptor;
};