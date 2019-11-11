#include "Block.h"

void Transaction::appendVin(vinType v) {
	vin.push_back(v);
	nTxin++;
}
void Transaction::appendVout(voutType v) {
	vout.push_back(v);
	nTxout++;
}

Block::Block()
{
	this->vTx.clear();
	this->height = -1;
	this->nonce = -1;
	this->blockid.clear();
	this->previousblockid.clear();
	this->merkleroot.clear();
	this->nTx = -1;

	this->selected = false;
	this->opened = false;
	this->showingInfo = false;
	this->showingTree = false;
	this->MerkleTree = -1;
	this->calculatedMerkleRoot.clear();
}

Block::Block(vector<Transaction> vTx, int height, int nonce, string blockid, string previousblockid, string merkleroot, int nTx)
{
	this->vTx = vTx;
	this->height = height;
	this->nonce = nonce;
	this->blockid = blockid;
	this->previousblockid = previousblockid;
	this->merkleroot = merkleroot;
	this->nTx = nTx;

	this->selected = false;
	this->opened = false;
	this->showingInfo = false;
	this->showingTree = false;
	this->MerkleTree = -1;
	this->calculatedMerkleRoot.clear();
}