#pragma once
#include <vector>
#include <string>

using namespace std;

typedef struct {
	string blockid;
	string txid;
	string signature;
	unsigned int nutxo;	//la posicion 1 se indica con 1 (no con cero)
}vinType;

typedef struct {
	string publicid;
	int amount;
}voutType;

class Transaction {
public:
	void appendVin(vinType v);
	void appendVout(voutType v);
	string txid;
	int nTxin;
	vector<vinType> vin;
	int nTxout;
	vector<voutType> vout;
	string publicid;	//lo agrego para poder validar las transacciones
};

class Filter_ {
public:
	string publicid;
	string ip;
	int port;
};

//bloque de la blockchain
class Block
{
public:
	Block();
	Block(vector<Transaction> vTx, int height, int nonce, string blockid, string previousblockid, string merkleroot, int nTx);

	//in JSON
	vector<Transaction> vTx;
	
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

