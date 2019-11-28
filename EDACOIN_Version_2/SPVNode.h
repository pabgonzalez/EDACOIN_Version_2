
#pragma once
#include "Node.h"
#include "Server.h"

class SPVNode: public Node
{
public:
	SPVNode(SocketType socket, string ID, map<string, SocketType> neighbourNodes = {});
	~SPVNode();

	void sendFilter(string nodeid);
	void getBlockHeader(string nodeid, string blockid);
	json generateFilter();

	void chooseTwoNeighbours();

	//Client
	void handleResponse();

	//Server
	void cycleConnections();

private:
	string respondToCommands(vector<string> commands, string uri, string method);
	string handlePOSTcommand(json j, string uri);
	Server* server;	//Servidor que recibe datos, luego de un filter
};