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
	int height;					//número de bloque
	int nonce;
	string blockid;
	string previousblockid;
	string merkleroot;
	int nTx;					//número de transacciones

	//internal
	bool selected;					//el bloque está seleccionado
	bool opened;					//el bloque está abierto 
	bool showingInfo;				//se está mostrando la info del bloque
	bool showingTree;				//se está mostrando el árbol
	int MerkleTree;					//índice en trees del árbol del bloque
	string calculatedMerkleRoot;	//merkle root calculada (en la raíz del árbol)
};

