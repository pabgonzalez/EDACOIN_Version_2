#pragma once
#include "Subject.h"
#include "FullNode.h"
#include "SPVNode.h"
#include "allegro5/allegro.h"


class FullNodeInfo {	//Nodo + info del nodo para la GUI y demas
public:
	FullNodeInfo(FullNode* n) {
		fullNode = n;
		neighbourID = "";
	}
	FullNode* fullNode;
	string neighbourID;		//Vecino(Full) al cual me voy a conectar
	int amount = 0;			//Unidades que se van a transferir
	string receiver = "";	//ID del nodo que recibe el pago
	string newip = "";		//IP del nuevo vecino
	int newport = 0;		//Port del nuevo vecino
	string newid = "";		//Id del nuevo vecino
	ALLEGRO_BITMAP* graphNeighbours(void);
	ALLEGRO_BITMAP* graph= NULL;
	bool graphi = false;
};

class SPVNodeInfo {
public:
	SPVNodeInfo(SPVNode* n) {
		spvNode = n;
		string neighbourID = "";
	}
	SPVNode* spvNode;
	string neighbourID;		//Vecino(Full) al cual me voy a conectar
	int amount = 0;			//Unidades que se van a transferir
	string receiver = "";	//ID del nodo que recibe el pago
	string newip = "";		//IP del nuevo vecino
	int newport = 0;		//Port del nuevo vecino
	string newid = "";		//Id del nuevo vecino
	string oldNeighbourID = "";	//ID del vecino que se reemplaza por otro
};

class LocalNodes : public Subject {
public:
	LocalNodes(string p);
	~LocalNodes();

	void setSelectedNode(int index, bool isFullNode);
	bool isSelectedNodeFull() { return selectedIsFull; }
	int getSelectedNodeIndex() { return selectedNodeIndex; }
	FullNodeInfo& getSelectedFullNode() { return getFullNode(selectedNodeIndex); }
	SPVNodeInfo& getSelectedSPVNode() { return getSPVNode(selectedNodeIndex); }
	bool isAnyNodeSelected();

	int getFullNodesSize() { return fullNodes.size(); }
	int getSPVNodesSize() { return spvNodes.size(); }
	FullNodeInfo& getFullNode(int i) { return fullNodes[i]; }
	SPVNodeInfo& getSPVNode(int i) { return spvNodes[i]; }

	void addFullNode(SocketType socket, string ID);
	void addSPVNode(SocketType socket, string ID);
	void appendNode(FullNode* node);
	void appendNode(SPVNode* node);

	void setManifestPath(string p) { manifestPath = p; }

protected:
	vector<FullNodeInfo> fullNodes;
	vector<SPVNodeInfo> spvNodes;

	string manifestPath;	//Lista de nodos de la red

	int selectedNodeIndex;
	bool selectedIsFull;
};