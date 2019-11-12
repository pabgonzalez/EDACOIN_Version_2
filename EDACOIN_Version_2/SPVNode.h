
#pragma once
#include "Node.h"

class SPVNode: public Node
{
public:
	void sendFilter(string nodeid);
	void getBlockHeader(string nodeid, string blockid);
	json generateFilter();

private:
};