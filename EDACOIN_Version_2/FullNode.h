#pragma once
#include "Node.h"

class FullNode : public Node
{
public:
	FullNode();
	void peer2peerNetFSM();
private:
};