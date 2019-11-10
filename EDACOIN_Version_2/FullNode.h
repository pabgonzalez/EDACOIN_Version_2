#pragma once
#include "Node.h"

typedef enum {IDLE, WAITING_LAYOUT, COLLECTING_NETWORK_MEMBERS, NETWORK_CREATED} p2pState;

class FullNode : public Node
{
public:
	FullNode();
	void p2pNetFSM();
private:
	bool pingStatus;
	unsigned timer;
	p2pState state;
};