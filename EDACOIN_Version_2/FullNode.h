#pragma once
#include "Node.h"
#include "Server.h"
#include "ServerRequest.h"
#include "ServerResponse.h"

using p2pstate = enum { IDLE = 0, WAITING_LAYOUT, COLLECTING_NETWORK_MEMBERS, NETWORK_CREATED };


class FullNode : public Node
{
public:
	FullNode(SocketType socket = { "", 0 }, string ID = "", map<string, SocketType> neighbourNodes = {});
	~FullNode();
	void p2pNetFSM();

	vector<vector<bool>> p2pAlgorithm(map<string, SocketType> Nodes);
	
	void cycleConnections();
private:
	vector<vector<bool>> p2pRecursive(vector<string>& IDs);
	void traverse(vector<bool>& visited, vector<vector<bool>> adjacencyMatrix, unsigned qNodes, int u = 0);
	bool isConnected(vector<vector<bool>> adjacencyMatrix, unsigned qNodes);
	int checkStrongConnections(vector<vector<bool>> matrix, int n);
	int countConnections(vector<vector<bool>> m, int i);
	bool checkFull(vector<vector<bool>> m, int n, int i);
	bool checkFullRare(vector<vector<bool>> m, int n, int i);
	bool checkFullEpic(vector<vector<bool>> m, int n, int i);
	bool pingStatus;
	unsigned timer;
	
	p2pstate state;
	vector<Server*> servers;
};