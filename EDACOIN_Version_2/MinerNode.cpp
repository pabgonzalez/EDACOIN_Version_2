#include "MinerNode.h"

void MinerNode::mineInit(void)
{
	srand(time(NULL));
	currBlock.height = blockChain.getBlockchainSize();
	currBlock.vTx = pendingTx;
	//calcular merkleroot
	//block.merkleroot = ...
	currBlock.previousblockid = blockChain.getPreviousBlockId(blockChain.getBlockchainSize() - 1);

	stringstream height;
	height << currBlock.height;
	currBlockStr = currBlock.previousblockid + height.str() + currBlock.merkleroot;
}


bool MinerNode::minecycle(void)
{
	string attempt = currBlockStr;
	int nonce = rand();
	stringstream ss;
	ss << nonce;
	string noncestr = ss.str();
	attempt += noncestr;

	CryptoPP::SHA256 hash;
	byte digest[CryptoPP::SHA256::DIGESTSIZE];
	hash.CalculateDigest(digest, (byte*)attempt.c_str(), attempt.length());

	CryptoPP::HexEncoder encoder;
	std::string hexid;
	encoder.Attach(new CryptoPP::StringSink(hexid));
	encoder.Put(digest, sizeof(digest));
	encoder.MessageEnd();

	string binaryid = hex_str_to_bin_str(hexid);

	int zeros = 0;
	for (int i = 0; i < challenge; i++) { zeros += binaryid[i]; }
	if (zeros == 0)
	{
		currBlock.nonce = nonce;
		return true;
	}
	else return false;
}