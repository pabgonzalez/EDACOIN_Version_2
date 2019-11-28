#include "Node.h"
#include <cryptopp/sha.h>
#include <cryptopp/sha3.h>
#include <cryptopp/hex.h>

using namespace std;

static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp);

Node::Node(SocketType socket, string ID, map<string, SocketType> neighbourNodes){
	this->socket = socket;
	this->neighbourNodes = neighbourNodes;
	this->ID = ID;

	//Client
	newResponse = false;
	httpResponse = "";
	httpMethod = "";
	performingFetch = 0;
	curl_global_init(CURL_GLOBAL_ALL);
}

Node::~Node() {
	curl_global_cleanup();
}

string Node::getNodeID() {
	return ID;
}

string Node::getNodeIP() {
	return socket.IP;
}

int Node::getNodePort() {
	return socket.port;
}

void Node::setManifestPath(string p) {
	manifestPath = p;
	manifestNodes = getNodesFromManifest();
}

map<string, SocketType> Node::getNodesFromManifest() {
	map<string, SocketType> nodes;
	std::ifstream manifestFile(manifestPath, std::ifstream::in);
	json j;
	manifestFile >> j;
	for (auto it : j)
	{
		nodes.insert(pair<string, SocketType>(it["id"], { it["ip"], it["port"] }));
	}
	return nodes;
}

void Node::setNodeSocket(SocketType s){
	socket = s;
}
void Node::appendNeighbourNode(string nID, SocketType nS){
	neighbourNodes.insert(pair<string, SocketType>(nID, nS));
}
void Node::appendNeighbourNode(Node neighbourNode){
	string nID = neighbourNode.getNodeID();
	SocketType nS = { neighbourNode.getNodeIP(), neighbourNode.getNodePort() };
	neighbourNodes.insert(pair<string, SocketType>(nID, nS));
}

void Node::sendTx(string nodeid, Transaction tx) {
	if (isNeighbour(nodeid)) {
		json j = generateTx(tx);
		string aux = j.dump();

		httpPost(nodeid, "/eda_coin/send_tx/", aux);
	}
}

void Node::httpPost(string nodeid, string addr, string msg) {
	httpPost(neighbourNodes[nodeid].IP, neighbourNodes[nodeid].port, addr, msg);
}

void Node::httpPost(string ip, int p, string addr, string msg, long timeout) {
	httpResponse = "";
	httpMethod = "POST";
	httpURI = addr;
	serverIP = ip;
	serverPort = p;

	curl = curl_easy_init();
	multiHandle = curl_multi_init();

	if ((curl != NULL) && (multiHandle != NULL)) {
		//Attacheo el easy handle para manejar una conexion no bloqueante.
		curl_multi_add_handle(multiHandle, curl);

		string url = ip + ":" + to_string(p);

		curl_easy_setopt(curl, CURLOPT_URL, (url + addr).c_str());
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTP);

		struct curl_slist* list = NULL;
		list = curl_slist_append(list, "Content-Type: application/x-www-form-urlencoded;charset=UTF-8");

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, msg.size());
		curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, msg.c_str());

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &httpResponse);

		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, timeout);

		performingFetch = 1;
	}
}

void Node::httpGet(string nodeid, string addr, string header) {
	httpResponse = "";
	httpMethod = "GET";
	httpURI = addr;
	serverIP = neighbourNodes[nodeid].IP;
	serverPort = neighbourNodes[nodeid].port;

	curl = curl_easy_init();
	multiHandle = curl_multi_init();

	if ((curl != NULL) && (multiHandle != NULL)) {
		//Attacheo el easy handle para manejar una conexion no bloqueante.
		curl_multi_add_handle(multiHandle, curl);

		string url = serverIP + ":" + to_string(serverPort);

		curl_easy_setopt(curl, CURLOPT_URL, (url + addr).c_str());
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTP);

		if (header.size() > 0) {
			struct curl_slist* list = NULL;
			list = curl_slist_append(list, header.c_str());
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
		}

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &httpResponse);

		performingFetch = 1;
	}
}

bool Node::performFetch() {
	//Realizamos un perform no bloqueante
	curl_multi_perform(multiHandle, &performingFetch);

	if (performingFetch == 0) {
		newResponse = true;
		cout << "Fin del perform, se recibio:" << endl;
		cout << httpResponse << endl;
		//Siempre realizamos el cleanup al final
		curl_easy_cleanup(curl);
	}

	//Devuelve true cuando finaliza el fetch
	return (performingFetch == 0) ? true : false;
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

json Node::generateTx(Transaction tx)
{
	//generate txid
	string s;
	s += tx.nTxin;
	vector<vinType>::iterator it;
	for (it = tx.vin.begin(); it != tx.vin.end(); it++) { s += it->blockid; }
	for (it = tx.vin.begin(); it != tx.vin.end(); it++) { s += it->txid; }
	for (it = tx.vin.begin(); it != tx.vin.end(); it++) { s += it->nutxo; }
	for (it = tx.vin.begin(); it != tx.vin.end(); it++) { s += it->signature; }
	vector<voutType>::iterator it2;
	for (it2 = tx.vout.begin(); it2 != tx.vout.end(); it2++) { s += it2->publicid;}
	for (it2 = tx.vout.begin(); it2 != tx.vout.end(); it2++) { s += it2->amount; }

	CryptoPP::SHA256 hash;
	byte digest[CryptoPP::SHA256::DIGESTSIZE];
	hash.CalculateDigest(digest, (byte*)s.c_str(), s.length());

	CryptoPP::HexEncoder encoder;
	std::string txid;
	encoder.Attach(new CryptoPP::StringSink(txid));
	encoder.Put(digest, sizeof(digest));
	encoder.MessageEnd();

	std::cout << "txid is: " << txid << std::endl;
	//***********************************************************************************//
	
	json j;

	//j["txid"] = tx.txid;
	j["txid"] = (string)txid;
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

bool Node::isNeighbour(string nodeid) {
	if (neighbourNodes.find(nodeid) == neighbourNodes.end()) {
		return false;
	}
	return true;
}

//Concatena lo recibido en content a s
static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	size_t realsize = size * nmemb;
	char* data = (char*)contents;
	string* s = (string*)userp;
	s->append(data, realsize);

	return realsize;
}
