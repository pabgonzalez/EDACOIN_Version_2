#pragma once

#include <iostream>
#include <cmath>
#include <boost/filesystem.hpp>
#include "Subject.h"
#include "Block.h"
#include <vector>
#include <algorithm>

#define BUFFER 16

using namespace std;
using namespace boost::filesystem;

//nodo del MerkleTree
class MerkleNode
{
public:
	MerkleNode()
	{
		newIDstr.clear();
		isReal = true;		//en principio no es un nodo copiado
		level = -1;
	}
	bool isReal;		//falso si es un nodo copiado
	string newIDstr;	//ID del nodo
	int level;			//nivel del nodo en el árbol
};

class BlockchainModel : public Subject
{
public:
	BlockchainModel();

	bool validateMerkleRoot(int i);
	void restart(void);

	int getFilesSize(void);
	string getFileName(int i);
	int getBlockchainSize(void);
	int getFileIndex(void);
	path getFilePath(int i) { return files[i]; }
	string getBlockId(int i) { return blockchain[i].blockid; }
	string getPreviousBlockId(int i) { return blockchain[i].previousblockid; }
	string getBlockMerkleRoot(int i) { return blockchain[i].merkleroot; }
	string getBlockCalculatedMerkleRoot(int i) { return blockchain[i].calculatedMerkleRoot; }
	int getBlockNonce(int i) { return blockchain[i].nonce; }
	int getBlockHeight(int i) { return blockchain[i].height; }
	int getBlockTransactionNumber(int i) { return blockchain[i].nTx; }
	bool getBlockSelected(int i) { return blockchain[i].selected; }
	bool getBlockOpened(int i) { return blockchain[i].opened; }
	bool getBlockShowingInfo(int i) { return blockchain[i].showingInfo; }
	bool getBlockShowingTree(int i) { return blockchain[i].showingTree; }
	vector<MerkleNode> getMerkleTree(int i) { return trees[blockchain[i].MerkleTree]; }
	bool getFileSelected() { return fileSelected; }
	//
	Transaction getTxInBlock(int b, int t) { return blockchain[b].vTx[t]; }
	//

	void setFileIndex(int fileIndex);
	void setFiles(vector<path> files);
	void clearBlockChain();
	void appendBlock(vector<Transaction> vTx, int height, int nonce, string blockid, string previousblockid, string merkleroot, int nTx);
	void setBlockSelected(int i, bool state);
	void setBlockOpened(int i, bool state);
	void setBlockShowingInfo(int i, bool state);
	void setBlockShowingTree(int i, bool state);
	void setFileSelected(bool b);

private:
	int createTree(vector<Transaction> vTx);
	int convertTransactionsToMerkleLeafs(vector<Transaction> vTx);
	vector<MerkleNode> propagateTreeUp(vector<MerkleNode> tree, vector<MerkleNode> level);
	void Print(vector<MerkleNode> tree);
	unsigned int generateID(unsigned char* str);

	vector<vector<MerkleNode>> trees;
	vector<Block> blockchain;
	vector<path> files;
	int fileIndex;
	bool fileSelected;
};

