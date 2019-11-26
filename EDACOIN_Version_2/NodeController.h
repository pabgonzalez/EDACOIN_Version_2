#pragma once
#include "Controller.h"
#include "LocalNodes.h"
#include "FullNode.h"
#include "SPVNode.h"

class NodeController : public Observer {
public:
	NodeController(LocalNodes& mod);
	void update(void*) {}
	void cycle(void);
	void showFullNodeGUI(FullNodeInfo& info);
	void showSPVNodeGUI(SPVNodeInfo& info);
private:
	LocalNodes& m;
};