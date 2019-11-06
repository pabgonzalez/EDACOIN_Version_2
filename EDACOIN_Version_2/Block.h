#pragma once

#include <vector>
#include <string>

using namespace std;

typedef struct {
	string blockid;
	string txid;
}vinType;

typedef struct {
	string publicid;
	int amount;
}voutType;

class Transaction {
public:
	void appendVin(vinType v);
	void appendVout(voutType v);
private:
	string txid;
	int nTxin;
	vector<vinType> vin;
	int nTxout;
	vector<voutType> vout;
};

//bloque de la blockchain
class Block
{
public:
	Block();
	Block(vector<string> vtx, int height, int nonce, string blockid, string previousblockid, string merkleroot, int nTx);

	//in JSON
	vector<string> vtx;			//vector de transacciones
	int height;					//n�mero de bloque
	int nonce;
	string blockid;
	string previousblockid;
	string merkleroot;
	int nTx;					//n�mero de transacciones

	//internal
	bool selected;					//el bloque est� seleccionado
	bool opened;					//el bloque est� abierto 
	bool showingInfo;				//se est� mostrando la info del bloque
	bool showingTree;				//se est� mostrando el �rbol
	int MerkleTree;					//�ndice en trees del �rbol del bloque
	string calculatedMerkleRoot;	//merkle root calculada (en la ra�z del �rbol)
};

