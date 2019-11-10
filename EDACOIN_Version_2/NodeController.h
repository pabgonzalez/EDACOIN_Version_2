#pragma once

#include "Controller.h"
#include "FullNode.h"
#include "SPVNode.h"

class NodeController : public Controller {
public:
	void update(void*) {}
	void cycle(void);
	void attach(FullNode* node) { fullNodes.push_back(node); requests.push_back(Request()); }
	void attach(SPVNode* node) { spvNodes.push_back(node); }
private:
	void getEvent(void) {};
	void dispatch(void) {};
	vector<FullNode*> fullNodes;
	vector<SPVNode*> spvNodes;
	vector<Request> requests;
};