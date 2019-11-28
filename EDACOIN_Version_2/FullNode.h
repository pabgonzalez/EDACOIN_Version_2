#pragma once
#include "Node.h"
#include "Server.h"
#include "BlockViewer.h"
#include <set>

using namespace std;

using p2pstate = enum { IDLE = 0, WAITING_LAYOUT, COLLECTING_NETWORK_MEMBERS, SENDING_LAYOUTS, NETWORK_CREATED };

class FullNode : public Node
{
public:
	FullNode(SocketType socket, string ID, map<string, SocketType> neighbourNodes = {});
	~FullNode();

	//setters
	void setManifestPath(string p);

	//getters
	BlockchainModel getBlockchain() { return blockChain; }

	//senders
	void sendPing(SocketType s);
	void sendLayout(SocketType s);
	void sendMerkleBlock(string nodeid, string blockid, string txid);
	void sendBlock(string nodeid, string blockid);

	//writters
	json generateBlockJson(string blockid);
	json generateMerkleBlock(string blockid, string txid);
	json generateBlockHeader(string blockid);
	string generateBlockChainJsonString();

	//crypto
	bool validateTx(Transaction tx);
	bool validateBlock(Block block, int challenge);
	string generateBlockID(Block block);

	//FSM
	void p2pFSM();
	json p2pAlgorithm(map<string, SocketType> Nodes);

	//Client
	void handleResponse();

	//Server
	void cycleConnections();

protected:
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

	int sendNextPing(); //Envia un Ping uno a uno a todos los restantes y devuelve cuantos faltan responder
	int sendNextLayout();	//Envia el layout al proximo nodo de los que resten
	void addNeighboursFromLayout(string layout); //TO-DO

	//flooding
	void floodTransaction(Transaction tx, string ip, int port);
	void floodBlock(Block b, string ip, int port);

	//crypto
	const char* hex_char_to_bin(char c);
	string hex_str_to_bin_str(const std::string& hex);

	//Server
	string respondToCommands(vector<string> commands, string uri, string method, string ip, int port);
	string handleGETcommand(string command, string uri, string ip, int port);
	string handlePOSTcommand(json j, string uri, string ip, int port);
	
	//unsigned timer;
	ALLEGRO_TIMER* timer = NULL;
	
	BlockchainModel blockChain;
	BlockViewer blockViewer;
	p2pstate state;
	vector<Server*> servers;
	set<string> subscriptors;
	vector<Transaction> pendingTx;

	map<string, SocketType>::iterator prevNodeIt;	//Iterador al nodo que recien pinguie/mande layout
	map<string, SocketType>::iterator nextNodeIt;	//Iterador al nodo que voy a pingear/mandar layout
	map<string, SocketType> onlineNodes;	//Lista de nodos que respondieron al ping
	string networkLayout;
	vector<Transaction> utxo;
};