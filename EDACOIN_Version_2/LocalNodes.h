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
	int getFullNodesSize() { return fullNodes.size(); }
	int getSPVNodesSize() { return spvNodes.size(); }
	FullNodeInfo& getFullNode(int i) { return fullNodes[i]; }
	SPVNodeInfo& getSPVNode(int i) { return spvNodes[i]; }

	void appendNode(FullNode* node) { fullNodes.push_back(FullNodeInfo(node)); notifyAllObservers(); }
	void appendNode(SPVNode* node) { spvNodes.push_back(SPVNodeInfo(node)); notifyAllObservers(); }
protected:
	vector<FullNodeInfo> fullNodes;
	vector<SPVNodeInfo> spvNodes;
};