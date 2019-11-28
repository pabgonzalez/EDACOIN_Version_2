#pragma once
#include "FullNode.h"

class MinerNode : FullNode
{
public:
	void validateAll(void);
	void generateCurrentMine(void);
	void setChallenge(int);
	bool checkChallenge(void);
private :
	int challenge = 3;
	string sha256;
	json currentMine;
	json waitList;
};