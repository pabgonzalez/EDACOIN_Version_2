#pragma once

#include "FullNode.h"
#include "SPVNode.h"

class NodeController : public Observer {
public:
	void update(void*) {}
	void cycle(void);
	void addNode(FullNode* node) { fullNodes.push_back(node); requests.push_back(Request()); }
	void addNode(SPVNode* node) { spvNodes.push_back(node); }
private:
	vector<FullNode*> fullNodes;
	vector<SPVNode*> spvNodes;
	vector<Request> requests;
};