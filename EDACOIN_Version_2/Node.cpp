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

void Node::sendBlock(string nodeid, string blockid) {
	json j = generateBlockJson(blockid);

	curl = curl_easy_init();
	CURLcode res;
	if (curl) {
		string url = neighbourNodes[nodeid].IP + ":" + to_string(neighbourNodes[nodeid].port);
		curl_easy_setopt(curl, CURLOPT_URL, url+ "/eda_coin/send_block/");
		string aux = j;
		//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, aux);
		//curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, postCallback);
		//curl_easy_setopt(curl, CURLOPT_WRITEDATA, &callbackData);

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
	//cout << j;
	return j;
}

json Node:: generateBlockHeader(string blockid)
{
	json j;
	//for (int i = 0; (blockChain.getBlockId(i) != blockid) && (i < blockChain.getBlockchainSize()); i++)
	for (int i = 0;  (i < 1); i++)
	//{
	//	j = {
	//			{ "height" , blockChain.getBlockHeight(i) },
	//			{ "nonce" , blockChain.getBlockNonce(i) },
	//			{ "blockid" , blockChain.getBlockId(i) },
	//			{ "previousblockid" , blockChain.getPreviousBlockId(i) },
	//			{ "merkleroot" , blockChain.getBlockMerkleRoot(i) },
	//			{ "nTx" , blockChain.getBlockTransactionNumber(i) }
	//	};
	//}
	{
		j = {
				{ "height" , 12 },
				{ "nonce" , 13 },
				{ "blockid" , "block" },
				{ "previousblockid" , "block-1" },
				{ "merkleroot" , "12345678" },
				{ "nTx" , 1 }
		};
	}
	cout << j;
	return j;
}