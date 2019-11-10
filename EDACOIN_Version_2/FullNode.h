#pragma once
#include "Node.h"

typedef enum {IDLE, WAITING_LAYOUT, COLLECTING_NETWORK_MEMBERS, NETWORK_CREATED} p2pState;

class FullNode : public Node
{
public:
	FullNode(SocketType socket, string ID, map<string, SocketType> neighbourNodes);
	void p2pNetFSM();
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
};