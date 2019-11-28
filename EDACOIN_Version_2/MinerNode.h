#pragma once
#include "FullNode.h"

class MinerNode : FullNode
{
public:
	void validateAll(void);
	void generateCurrentMine(void);
	void setChallenge(int);
	bool checkChallenge(void);

	void mineInit(void);	//cada vez que se empieza a minar
	bool minecycle(void);	//ada vez que se intenta minar, devuelve true si logro minar currBlock
private :
	int challenge = 3;
	string sha256;
	json currentMine;
	json waitList;

	string currBlockStr;
	Block currBlock;
};


