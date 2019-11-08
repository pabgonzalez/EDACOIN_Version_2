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
	Node(SocketType socket, string ID, map<string, SocketType> neighbourNodes = {} );
	~Node();
	//appendNeighbourNode()
	
	//getters
	string getNodeIP();
	int getNodePort();
	SocketType getNeighbourSockets(string ID);
	
	//Senders (POST)
	void sendBlock(string nodeid, string blockid);
	void sendTx(string nodeid, Transaction tx);

	//setters
	void setNodeSocket(SocketType socket);
	void appendNeighbourSocket(string neighbourID, SocketType neighbourSocket);
	
	//writers
	json generateBlockJson(string blockid);
	json generateMerkleBlock(string blockid, string txid);
	json generateBlockHeader(string blockid);
	json generateFilter();
	json generateTx(Transaction tx);

	
private:
	vector<string> recursiveMerkleBlock(vector<MerkleNode> t, int pos);

	string ID;
	SocketType socket;
	map<string,SocketType> neighbourNodes;
	BlockchainModel blockChain;

	CURL* curl;
	string callbackData;
	size_t postCallback(char* contents, size_t size, size_t nmemb, void* userData);
};
