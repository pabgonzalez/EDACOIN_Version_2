#include "LocalNodes.h"

LocalNodes::LocalNodes(string p) {
	selectedNodeIndex = -1;
	selectedIsFull = false;
	manifestPath = p;
}

LocalNodes::~LocalNodes() {
	for (unsigned int i = 0; i < fullNodes.size(); i++) {
		delete fullNodes[i].fullNode;
	}
	for (unsigned int i = 0; i < spvNodes.size(); i++) {
		delete spvNodes[i].spvNode;
	}
}

bool LocalNodes::isAnyNodeSelected() {
	if (selectedNodeIndex >= 0) {
		if (selectedIsFull && fullNodes.size() > selectedNodeIndex){
			return true;
		}
		if (!selectedIsFull && spvNodes.size() > selectedNodeIndex) {
			return true;
		}
	}
	return false;
}

void LocalNodes::setSelectedNode(int index, bool isFullNode) {
	selectedNodeIndex = index;
	selectedIsFull = isFullNode;
	notifyAllObservers();
}

void LocalNodes::addFullNode(SocketType socket, string ID) {
	FullNode* full = new FullNode(socket, ID);
	full->setManifestPath(manifestPath);
	appendNode(full);
	notifyAllObservers();
}
void LocalNodes::addSPVNode(SocketType socket, string ID) {
	SPVNode* spv = new SPVNode(socket, ID);
	spv->setManifestPath(manifestPath);
	spv->chooseTwoNeighbours();
	appendNode(spv);
	notifyAllObservers();
}

void LocalNodes::appendNode(FullNode* node) {
	fullNodes.push_back(FullNodeInfo(node));
	notifyAllObservers();
}

void LocalNodes::appendNode(SPVNode* node) {
	spvNodes.push_back(SPVNodeInfo(node));
	notifyAllObservers();
}