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
	FullNode(SocketType socket = { "", 0 }, string ID = "", map<string, SocketType> neighbourNodes = {});
	~FullNode();
	void p2pNetFSM();

	void acceptConnection();
	void writeResponse();
	void readRequest();
	vector<vector<bool>> p2pAlgorithm(map<string, SocketType> Nodes);
	

private:
	vector<vector<bool>> p2pRecursive(vector<string>& IDs);
	void traverse(vector<bool>& visited, vector<vector<bool>> adjacencyMatrix, unsigned qNodes, int u = 0);
	bool isConnected(vector<vector<bool>> adjacencyMatrix, unsigned qNodes);
	int checkStrongConnections(vector<vector<bool>> matrix, int n);
	int countConnections(vector<vector<bool>> m, int i);
	bool checkFull(vector<vector<bool>> m, int n, int i);
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