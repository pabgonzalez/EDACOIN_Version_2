#pragma once
#include <iostream>
#include <cstdio>
#include <cmath>
#include "Subject.h"
#include <vector>
#include <algorithm>
#include <map>
#include "json.hpp"
#include <fstream>
#include "BlockchainModel.h"
#define CURL_STATICLIB
#include <curl\curl.h>

#include <cryptopp/sha.h>
#include <cryptopp/eccrypto.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha.h>
#include <cryptopp/sha3.h>
#include <cryptopp/hex.h>
#include <cryptopp/oids.h>

using namespace std;
using json = nlohmann::json;
using namespace CryptoPP;

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
	bool getNewResponse() { return newResponse; }
	SocketType getNeighbourSockets(string ID);
	string getResponse() { return httpResponse; }
	string getHttpMethod() { return httpMethod; }
	string getServerIP() { return serverIP; }
	int getServerPort() { return serverPort; }
	string getHttpURI() { return httpURI; }
	bool isPerformingFetch() { return (performingFetch == 0)? false : true; }
	map<string, SocketType> getNeighbours() { return neighbourNodes; }
	map<string, SocketType> getNodesFromManifest();
	
	//Senders (POST)
	void sendTx(string nodeid, Transaction tx);
	void httpPost(string nodeid, string addr, string msg);
	void httpPost(string ip, int p, string addr, string msg, long timeout = 10000);

	//Requesters (GET)
	void httpGet(string ip, int p, string addr, string header = "");
	void httpGet(string nodeid, string addr, string header = "");

	//setters
	void setNodeSocket(SocketType socket);
	void setManifestPath(string p);
	void setNewResponse(bool b) { newResponse = b; }

	//appenders
	void appendNeighbourNode(string neighbourID, SocketType neighbourSocket);
	void appendNeighbourNode(Node neighbourNode);
	
	//writers
	json generateTx(Transaction tx);

	//crypto
	string generateTxid(Transaction tx);

	//otros
	bool isNeighbour(string nodeid);
	void removeNeighbourNode(string nodeid) { neighbourNodes.erase(nodeid); }

	//curl functions
	bool performFetch();
	
protected:
	string ID;
	SocketType socket;
	map<string,SocketType> neighbourNodes;

	//Client side
	CURL* curl;
	CURLM* multiHandle;
	bool newResponse;	//Hay una respuesta nueva para atender
	string httpResponse;
	int performingFetch;
	string httpMethod;	//Vale POST O GET
	string httpURI;
	string serverIP;
	int serverPort;

	ECDSA<ECP, SHA256>::PrivateKey privateKey;

	string manifestPath;	//Lista de nodos de la red
	map<string, SocketType> manifestNodes;	//Lista de nodos del manifiesto
};
