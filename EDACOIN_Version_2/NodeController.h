#pragma once

#include "FullNode.h"
#include "SPVNode.h"

class NodeController{
public:
	void cycle(void);
	void attach(FullNode* node) { fullNodes.push_back(node); requests.push_back(Request()); }
	void attach(SPVNode* node) { spvNodes.push_back(node); }
private:
	vector<FullNode*> fullNodes;
	vector<SPVNode*> spvNodes;
	vector<Request> requests;
};