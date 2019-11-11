#pragma once
#include <iostream>
#include <cstdio>
#include <cmath>
#include "Subject.h"
#include <vector>
#include <algorithm>
#include <map>
#include "json.hpp"
#include "BlockchainModel.h"
#define CURL_STATICLIB
#include <curl\curl.h>

using namespace std;
using json = nlohmann::json;

typedef struct {
	string IP;
	int port;
}SocketType;

class Node : public Subject
{
public:
	//constructores
	Node(SocketType socket = {"", 0}, string ID = "", map<string, SocketType> neighbourNodes = {});
	
	//destructores
	~Node();
	
	//getters
	string getNodeID();
	string getNodeIP();
	int getNodePort();
	SocketType getNeighbourSockets(string ID);
	bool isPerformingFetch() { return (performingFetch == 0)? false : true; }
	
	//Senders (POST)
	void sendBlock(string nodeid, string blockid);
	void sendTx(string nodeid, Transaction tx);
	void httpPost(string nodeid, string addr, string msg);

	//Requesters (GET)
	void httpGet(string nodeid, string addr);

	//setters
	void setNodeSocket(SocketType socket);
	
	//appenders
	void appendNeighbourNode(string neighbourID, SocketType neighbourSocket);
	void appendNeighbourNode(Node neighbourNode);
	
	//writers
	json generateBlockJson(string blockid);
	json generateMerkleBlock(string blockid, string txid);
	json generateBlockHeader(string blockid);
	json generateFilter();
	json generateTx(Transaction tx);

	//curl functions
	bool performFetch();
	
protected:
	vector<string> recursiveMerkleBlock(vector<MerkleNode> t, int pos);

	string ID;
	SocketType socket;
	map<string,SocketType> neighbourNodes;
	BlockchainModel blockChain;

	//Client side
	CURL* curl;
	CURLM* multiHandle;
	string httpResponse;
	int performingFetch;
};
