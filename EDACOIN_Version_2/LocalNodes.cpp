#include "LocalNodes.h"

LocalNodes::~LocalNodes() {
	for (unsigned int i = 0; i < fullNodes.size(); i++) {
		delete fullNodes[i].fullNode;
	}
	for (unsigned int i = 0; i < spvNodes.size(); i++) {
		delete spvNodes[i].spvNode;
	}
}

void LocalNodes::addFullNode(SocketType socket, string ID, map<string, SocketType> neighbourNodes) {
	FullNode* full = new FullNode(socket, ID, neighbourNodes);
	appendNode(full);
	notifyAllObservers();
}
void LocalNodes::addSPVNode(SocketType socket, string ID, map<string, SocketType> neighbourNodes) {
	SPVNode* spv = new SPVNode(socket, ID, neighbourNodes);
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