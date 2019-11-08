#include "Node.h"

using namespace std;

Node::Node(SocketType socket, string ID, map<string, SocketType> neighbourNodes){
	this->socket = socket;
	this->neighbourNodes = neighbourNodes;
	this->ID = ID;

	curl_global_init(CURL_GLOBAL_ALL);
}

Node::~Node() {
	curl_global_cleanup();
}

string Node::getNodeIP() {
	return socket.IP;
}

int Node::getNodePort() {
	return socket.port;
}
void Node::setNodeSocket(SocketType s){
	socket = s;
}
void Node::appendNeighbourNode(string nID, SocketType nS){
	neighbourNodes.insert(pair<string, SocketType>(nID, nS));
}

void Node::sendBlock(string nodeid, string blockid) {
	json j = generateBlockJson(blockid);

	curl = curl_easy_init();
	CURLcode res;
	if (curl) {
		string url = neighbourNodes[nodeid].IP + ":" + to_string(neighbourNodes[nodeid].port);
		curl_easy_setopt(curl, CURLOPT_URL, url+ "/eda_coin/send_block/");
		string aux = j;
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, aux);
		//curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, postCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &callbackData);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		curl_easy_cleanup(curl);
	}
}

void Node::sendTx(string nodeid, Transaction tx) {
	json j = generateTx(tx);

	curl = curl_easy_init();
	CURLcode res;
	if (curl) {
		string url = neighbourNodes[nodeid].IP + ":" + to_string(neighbourNodes[nodeid].port);
		curl_easy_setopt(curl, CURLOPT_URL, url + "/eda_coin/send_tx/");
		string aux = j;
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, aux);
		//curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, postCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &callbackData);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		curl_easy_cleanup(curl);
	}
}

SocketType Node::getNeighbourSockets(string id) {
	SocketType ret;
	map<string, SocketType>::iterator it;
	it = neighbourNodes.find(id);
	if (it != neighbourNodes.end())
		ret = it->second;
	else {
		SocketType nullSocket = { "", 0 };
		ret = nullSocket;
	}
	return ret;
}

json Node::generateFilter() {
	json j;
	j["Id"] = ID;
	return j;
}

json Node::generateBlockJson(string blockid) 
{
	int b;
	for (b = 0; b < blockChain.getBlockchainSize(); b++)
	{
		if (blockChain.getBlockId(b) == blockid) { break; }
	}

	json j;
	for (int t = 0; t < blockChain.getBlockTransactionNumber(b); t++)
	{
		j["tx"] += generateTx(blockChain.getTxInBlock(b, t));
	}
	
	j["height"] = blockChain.getBlockHeight(b);
	j["nonce"] = blockChain.getBlockNonce(b);
	j["blockid"] = blockid;
	j["previousblockid"] = blockChain.getPreviousBlockId(b);
	j["merkleroot"] = blockChain.getBlockMerkleRoot(b);
	j["nTx"] = blockChain.getBlockTransactionNumber(b);
	return j;
}

json Node::generateMerkleBlock(string blockid , string txid)
{
	int indexB, indexT;
	for ( indexB = 0; indexB < blockChain.getBlockchainSize(); indexB++)
	{
		if (blockid == blockChain.getBlockId(indexB))
		{
			for (indexT = 0; indexT < blockChain.getBlockTransactionNumber(indexB); indexT++)
			{
				if (txid == blockChain.getTxInBlock(indexB, indexT).txid) { break; }
			}
			break;
		}
	}
	json j;
	j["blockid"] = blockid;
	j["tx"] = generateTx( blockChain.getTxInBlock(indexB,indexT) );
	j["txPos"] = indexT;	//las tx empiezan en pos 0

	vector<string> Ids = recursiveMerkleBlock( blockChain.getMerkleTree(indexB), indexT);	//arbol de bloque
	for (int i = 0; i < Ids.size(); i++)
	{
		j["merklePath"] += { {"Id", Ids[i]} };
	}

	return j;
}

vector<string> Node:: recursiveMerkleBlock(vector<MerkleNode> t, int pos)
{
	static vector<string> Ids;			//vector de IDstring de los nodos necesarios
	static int level = t[0].level;		//nivel actual

	if (level == 0) { return Ids; }		//
	
	(pos % 2) ? Ids.push_back(t[pos - 1].newIDstr) : Ids.push_back(t[pos + 1].newIDstr);	//guardo el IDstring respectivo
	
	while (t[0].level == level) { t.erase(t.begin()); }	//borro el nivel actual

	level--;	//paso al siguiente nivel

	return recursiveMerkleBlock(t, pos/2);
}

json Node::generateTx(Transaction tx)
{
	json j;

	j["txid"] = tx.txid;
	j["nTxin"] = tx.nTxin;
	for (int i = 0; i < tx.nTxin; i++)
	{
		j["vin"] += { {"blockid", tx.vin[i].blockid}, { "txid", tx.vin[i].txid } };
	}
	j["nTxout"] = tx.nTxout;
	for (int i = 0; i < tx.nTxout; i++)
	{
		j["vout"] += { {"publicid", tx.vout[i].publicid}, { "amount", tx.vout[i].amount } };
	}
	return j;
}

//generateBlockHeader genera un json (array de objectos), donde cada objeto es el header de cada bloque hasta el indicado por blockid
json Node:: generateBlockHeader(string blockid)
{
	// create JSON 
	json j;
	for (int i = 0; (blockChain.getBlockId(i) != blockid) && (i < blockChain.getBlockchainSize()); i++) 
	{
		// add values
		j += { 
				{ "height", blockChain.getBlockHeight(i) }, 
				{ "nonce", blockChain.getBlockNonce(i) },
				{ "blockid", blockChain.getBlockId(i) },
				{ "previousblockid", blockChain.getPreviousBlockId(i) },
				{ "merkleroot", blockChain.getBlockMerkleRoot(i) },
				{ "nTx", blockChain.getBlockTransactionNumber(i) }
			};
	}
	// print values
	cout << j << endl;

	return j;
}