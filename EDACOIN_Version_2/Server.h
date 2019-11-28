#pragma once
#include <iostream>
#include <cstdio>
#include <cstring>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#define EXPIRE_TIME     30                  // En segundos
#define HOST_NAME       "localhost"         // Host named esperado
#define HOST_NAME_LEN   9

using namespace std;

class Server
{
public:
	Server(unsigned int port);
	~Server();
	bool acceptConnection();	//Acepta la conexion y pasa a readingRequest
	bool readRequest();			//Lee el mensaje y lo guarda en receivedMessage
	bool writeResponse();
	void sendResponse(string status, string content);	//Envia la respuesta deseada

	string getRequest();		//Devuelve receivedMessage
	string getURI() { return uri; }
	string getMethod() { return method; }
	int getCommandAmount() { return commands.size(); }
	vector<string> getCommands() { return commands; }
	string getCommand(int i) { return commands[i]; }
	string getClientIP();
	int getClientPort();

private:
	void parseHttpRequest(string request);
	void findURIandMethod(string command, string& uri, string& method);
	char* getCurrentDateString(int delay = 0);

	string clientIP;
	int clientPort;
	string uri;
	string method;
	vector<string> commands;
	string receivedMessage;
	bool readingRequest;
	bool requestReady;
	bool writingResponse;
	string response;
	boost::asio::io_service* IO_Handler;
	boost::asio::ip::tcp::socket* serverSocket;
	boost::asio::ip::tcp::acceptor* serverAcceptor;
};