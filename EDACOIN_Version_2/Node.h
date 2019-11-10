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
	string getNodeIP();
	int getNodePort();
	SocketType getNeighbourSockets(string ID);
	
	//Senders (POST)
	void sendBlock(string nodeid, string blockid);
	void sendTx(string nodeid, Transaction tx);

	//setters
	void setNodeSocket(SocketType socket);
	
	//appenders
	void appendNeighbourNode(string neighbourID, SocketType neighbourSocket);
	
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

	//Client side
	CURL* curl;
	string callbackData;
	string httpResponse;
	size_t writeCallback(char* contents, size_t size, size_t nmemb, void* userData);
};
