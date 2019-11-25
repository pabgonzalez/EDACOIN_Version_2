#pragma once
#include "Subject.h"
#include "FullNode.h"
#include "SPVNode.h"

class FullNodeInfo {	//Nodo + info del nodo para la GUI y demas
public:
	FullNodeInfo(FullNode* n) {
		fullNode = n;
		neighbourID = "";
	}
	FullNode* fullNode;
	string neighbourID; //Vecino al cual me voy a conectar
	int amount = 0;	//Unidades para transferir
};

class SPVNodeInfo {
public:
	SPVNodeInfo(SPVNode* n) {
		spvNode = n;
		string neighbourID = "";
	}
	SPVNode* spvNode;
	string neighbourID; //Vecino al cual me voy a conectar
	int amount = 0;
};

class LocalNodes : public Subject {
public:
	~LocalNodes();

	int getFullNodesSize() { return fullNodes.size(); }
	int getSPVNodesSize() { return spvNodes.size(); }
	FullNodeInfo& getFullNode(int i) { return fullNodes[i]; }
	SPVNodeInfo& getSPVNode(int i) { return spvNodes[i]; }

	void addFullNode(SocketType socket, string ID, map<string, SocketType> neighbourNodes);
	void addSPVNode(SocketType socket, string ID, map<string, SocketType> neighbourNodes);
	void appendNode(FullNode* node);
	void appendNode(SPVNode* node);
protected:
	vector<FullNodeInfo> fullNodes;
	vector<SPVNodeInfo> spvNodes;
};