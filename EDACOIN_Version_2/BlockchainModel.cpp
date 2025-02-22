#include "BlockchainModel.h"
#include <iostream>
#include <cmath>

#include <cryptopp/sha.h>
#include <cryptopp/eccrypto.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha.h>
#include <cryptopp/sha3.h>
#include <cryptopp/hex.h>
#include <cryptopp/oids.h>

BlockchainModel::BlockchainModel() {
	fileIndex = -1;
	fileSelected = false;
	restart();
}

bool BlockchainModel::validateMerkleRoot(int i)
{
	return (blockchain[i].calculatedMerkleRoot == blockchain[i].merkleroot) ? true : false;
}

void BlockchainModel::setFiles(vector<path> files) {
	this->files = files;
	notifyAllObservers();
}

void BlockchainModel::appendBlock(vector<Transaction> vTx, int height, int nonce, string blockid, string previousblockid, string merkleroot, int nTx)
{
	Block block(vTx, height, nonce, blockid, previousblockid, merkleroot, nTx);

	block.MerkleTree = createTree(vTx);

	if (trees[block.MerkleTree].size() > 0)
		block.calculatedMerkleRoot = trees[block.MerkleTree].back().newIDstr;
	blockchain.push_back(block);
	notifyAllObservers();
}

void BlockchainModel::appendBlock(Block block)
{
	block.MerkleTree = createTree(block.vTx);

	if (trees[block.MerkleTree].size() > 0)
		block.calculatedMerkleRoot = trees[block.MerkleTree].back().newIDstr;
	blockchain.push_back(block);
	notifyAllObservers();
}

void BlockchainModel::setFileIndex(int fileIndex)
{
	this->fileIndex = fileIndex;
	notifyAllObservers();
}

void BlockchainModel::setFileSelected(bool b) {
	fileSelected = b;
	notifyAllObservers();
}

void BlockchainModel::clearBlockChain() {
	blockchain.clear();
	trees.clear();
	notifyAllObservers();
}

void BlockchainModel::setBlockSelected(int i, bool state) {
	blockchain[i].selected = state;
	notifyAllObservers();
}

void BlockchainModel::setBlockOpened(int i, bool state) {
	blockchain[i].opened = state;
	notifyAllObservers();
}

void BlockchainModel::setBlockShowingInfo(int i, bool state) {
	blockchain[i].showingInfo = state;
	notifyAllObservers();
}

void BlockchainModel::setBlockShowingTree(int i, bool state) {
	blockchain[i].showingTree = state;
	notifyAllObservers();
}

int BlockchainModel::getFilesSize(void)
{
	return (int)files.size();
}

string BlockchainModel::getFileName(int i)
{
	return files[i].filename().string();
}

int BlockchainModel::getBlockchainSize(void)
{
	return (int)blockchain.size();
}

int BlockchainModel::getFileIndex(void)
{
	return fileIndex;
}

void BlockchainModel::restart(void)
{
	files.clear();
	blockchain.clear();
	fileIndex = -1;
	fileSelected = false;
	notifyAllObservers();
}

int BlockchainModel::createTree(vector<Transaction> vTx)
{
	int index = convertTransactionsToMerkleLeafs(vTx);
	trees.back() = propagateTreeUp(trees.back(), trees.back());
	return index;
}

int BlockchainModel::convertTransactionsToMerkleLeafs(vector<Transaction> vTx)
{
	vector<MerkleNode> leafs;
	for (unsigned int i = 0; i < vTx.size(); i++)
	{
		
		//unsigned int newID = generateID((unsigned char*)vTx[i].txid.c_str());	//id del las hojas
		//char aux[BUFFER] = { '\0' };
		//sprintf_s(aux, sizeof(aux), "%08X", newID);		//pasa a ASCII
		//if (strlen(aux) != 8)
		//{
		//	cout << "Error in length LEAF" << endl;
		//}
		MerkleNode mn;
		//mn.newIDstr = aux;
		mn.newIDstr = vTx[i].txid;
		mn.isReal = true;
		mn.level = (int)ceil(log2(vTx.size()));
		leafs.push_back(mn);
	}
	trees.push_back(leafs);
	return (int)trees.size() - 1;
}

vector<MerkleNode> BlockchainModel::propagateTreeUp(vector<MerkleNode> tree, vector<MerkleNode> level)
{
	vector<MerkleNode> nextLevel;
	if (level.size() == 0 || level.size() == 1)
	{
		return tree;
	}
	else if (level.size() % 2)
	{
		level.push_back(level.back());
		level.back().isReal = false;
		tree.push_back(level.back());
	}

	for (unsigned int i = 0; i < level.size(); i += 2)
	{
		string conc = level[i].newIDstr + level[i + 1].newIDstr;		//toma el par
		//unsigned int newID = generateID((unsigned char*)conc.c_str());		//id del padre
		//char temp[BUFFER] = { '\0' };
		//sprintf_s(temp, sizeof(temp), "%08X", newID);		//pasa a ASCII
		//if (strlen(temp) != 8)
		//{
		//	cout << "Error in length " << endl;
		//}
		CryptoPP::SHA256 hash;
		uint8_t digest[CryptoPP::SHA256::DIGESTSIZE];
		hash.CalculateDigest(digest, (uint8_t*)conc.c_str(), conc.length());

		CryptoPP::HexEncoder encoder;
		std::string parent;
		encoder.Attach(new CryptoPP::StringSink(parent));
		encoder.Put(digest, sizeof(digest));
		encoder.MessageEnd();

		MerkleNode mn;
		//mn.newIDstr = temp;
		mn.newIDstr = parent;
		mn.level = level[i].level - 1;

		/*if (level[i].isReal == true)
		{
			mn.l = &tree[tree.size() - level.size() + i];
			mn.isReal = true;
			cout << "L : " << mn.l << endl;
		}
		if (level[i + 1].isReal == true)
		{
			mn.r = &tree[tree.size() - level.size() + i + 1];
			cout << "R : " << mn.r << endl;
		}*/

		nextLevel.push_back(mn);
		tree.push_back(nextLevel.back());
	}

	return propagateTreeUp(tree, nextLevel);
}

//unsigned int BlockchainModel::generateID(unsigned char* str)
//{
//	unsigned int ID = 0;
//	int c;
//	while (c = *str++)
//		ID = c + (ID << 6) + (ID << 16) - ID;
//	return ID;
//}

void BlockchainModel::Print(vector<MerkleNode> tree)
{
	cout << "Level\tID\t\tisReal" << endl;
	for (unsigned int i = 0; i < tree.size(); i++)
	{
		cout << tree[i].level << '\t';
		cout << tree[i].newIDstr << '\t';
		cout << tree[i].isReal << endl;
	}
}