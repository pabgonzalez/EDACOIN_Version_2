#pragma once
#include "Node.h"
#include "Server.h"
#include <set>

using namespace std;

using p2pstate = enum { IDLE = 0, WAITING_LAYOUT, COLLECTING_NETWORK_MEMBERS, NETWORK_CREATED };

class FullNode : public Node
{
public:
	FullNode(SocketType socket = { "", 0 }, string ID = "", map<string, SocketType> neighbourNodes = {});
	~FullNode();
	
	//getters
	BlockchainModel getBlockchain() { return blockChain; }

	//senders
	void sendMerkleBlock(string nodeid, string blockid, string txid);
	void sendBlock(string nodeid, string blockid);

	//writters
	json generateBlockJson(string blockid);
	json generateMerkleBlock(string blockid, string txid);
	json generateBlockHeader(string blockid);

	//FSM
	void p2pNetFSM();
	vector<vector<bool>> p2pAlgorithm(map<string, SocketType> Nodes);

	//Server
	void cycleConnections();

private:
	//recursives
	vector<string> recursiveMerkleBlock(vector<MerkleNode> t, int pos);

	//p2p
	void traverse(vector<bool>& visited, vector<vector<bool>> adjacencyMatrix, unsigned qNodes, int u = 0);
	bool isConnected(vector<vector<bool>> adjacencyMatrix, unsigned qNodes);
	int countConnections(vector<vector<bool>> m, int i);
	int checkStrongConnections(vector<vector<bool>> matrix, int n);
	bool checkFull(vector<vector<bool>> m, int n, int i);
	bool checkFullRare(vector<vector<bool>> m, int n, int i);
	bool checkFullEpic(vector<vector<bool>> m, int n, int i);

	//Server
	string handleHttpRequest(string request);
	void findURIandMethod(string command, string& uri, string& method);
	string handleGETcommand(string command, string uri);
	string handlePOSTcommand(json j, string uri);
	
	bool pingStatus;
	unsigned timer;
	BlockchainModel blockChain;
	p2pstate state;
	vector<Server*> servers;
	set<string> subscriptors;
	vector<Transaction> pendingTx;
};