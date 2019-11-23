#include "FullNode.h"
#include "JsonController.h"
#include <chrono>
#include <thread>

using namespace std;

FullNode::FullNode(SocketType socket, string ID, map<string, SocketType> neighbourNodes)
{
	this->socket = socket;
	this->ID = ID;
	this->neighbourNodes = neighbourNodes;
	pingStatus = false;
	timer = (rand() % 999) * 10 + 10;
	state = IDLE;

	Server* newserver = new Server(socket.port);
	servers.push_back(newserver);
}

FullNode::~FullNode() {
	for (unsigned int i = 0; i < servers.size(); i++) {
		delete servers[i];
	}
}

void FullNode::sendMerkleBlock(string nodeid, string blockid, string txid) {
	if (isNeighbour(nodeid)) {
		json j = generateMerkleBlock(blockid, txid);
		string aux = j;

		httpPost(nodeid, "/eda_coin/send_merkle_block/", j);
	}
}

void FullNode::sendBlock(string nodeid, string blockid) {
	if (isNeighbour(nodeid)) {
		json j = generateBlockJson(blockid);
		string aux = j;

		httpPost(nodeid, "/eda_coin/send_block/", j);
	}
}

json FullNode::generateBlockJson(string blockid)
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

//generateBlockHeader genera un json (array de objectos), donde cada objeto es el header de cada bloque hasta el indicado por blockid
json FullNode::generateBlockHeader(string blockid)
{
	// create JSON 
	json j;
	for (int i = 0; i < blockChain.getBlockchainSize(); i++)
	{
		if (blockChain.getBlockId(i) == blockid) {
			// add values
			j += {
				{ "height", blockChain.getBlockHeight(i) },
				{ "nonce", blockChain.getBlockNonce(i) },
				{ "blockid", blockChain.getBlockId(i) },
				{ "previousblockid", blockChain.getPreviousBlockId(i) },
				{ "merkleroot", blockChain.getBlockMerkleRoot(i) },
				{ "nTx", blockChain.getBlockTransactionNumber(i) }
			};
			break;
		}
	}
	return j;
}

json FullNode::generateMerkleBlock(string blockid, string txid)
{
	int indexB, indexT;
	for (indexB = 0; indexB < blockChain.getBlockchainSize(); indexB++)
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
	j["tx"] = generateTx(blockChain.getTxInBlock(indexB, indexT));
	j["txPos"] = indexT;	//las tx empiezan en pos 0

	vector<string> Ids = recursiveMerkleBlock(blockChain.getMerkleTree(indexB), indexT);	//arbol de bloque
	for (unsigned int i = 0; i < Ids.size(); i++)
	{
		j["merklePath"] += { {"Id", Ids[i]} };
	}
	return j;
}

vector<string> FullNode::recursiveMerkleBlock(vector<MerkleNode> t, int pos)
{
	static vector<string> Ids;			//vector de IDstring de los nodos necesarios
	static int level = t[0].level;		//nivel actual

	if (level == 0) { return Ids; }		//

	(pos % 2) ? Ids.push_back(t[pos - 1].newIDstr) : Ids.push_back(t[pos + 1].newIDstr);	//guardo el IDstring respectivo

	while (t[0].level == level) { t.erase(t.begin()); }	//borro el nivel actual

	level--;	//paso al siguiente nivel

	return recursiveMerkleBlock(t, pos / 2);
}

void FullNode::p2pNetFSM() {
	switch (state) {
	case IDLE:
		if (pingStatus == true)
			state = WAITING_LAYOUT;
		else {
			this_thread::sleep_for(chrono::milliseconds(1));
			if (!(--timer))
				state = COLLECTING_NETWORK_MEMBERS;
		}
		break;
		//case WAITING_LAYOUT:
		//	//epera mensaje NetwokLayout
		//	//responde HTTP OK
		//	//agrega vecinos
		//case COLLECTING_NETWORK_MEMBERS:
		//	//envia PING a cada nodo
		//	//si NetworkNotReady agrega nodo a lista
		//	//si NetworkReady toma lista y conecta con la actual
		//case NETWORK_CREATED:
		//	//
	}
}

vector<vector<bool>> FullNode::p2pAlgorithm(map<string, SocketType> Nodes)
{
	vector<string> IDs;
	for (auto const& element : Nodes)
		IDs.push_back(element.first);
	IDs.push_back(this->ID);
	
	vector<vector<bool>> adjacencyMatrix(IDs.size(), vector<bool>(IDs.size(), 0));
	while (isConnected(adjacencyMatrix, IDs.size()) == false || checkStrongConnections(adjacencyMatrix, IDs.size()) != -1)
	{
		unsigned i = rand() % IDs.size(), j = i;
		cout << i << endl;
		if (countConnections(adjacencyMatrix, i) <= 2)	//si tiene menos de dos conexiones le busco una nuevo conexión
		{
			while (j == i || adjacencyMatrix[i][j] == true || countConnections(adjacencyMatrix, j) > 3)
			{
				j = rand() % IDs.size();
				if (checkFull(adjacencyMatrix, IDs.size(), i) == true)
				{
					if (j != i && adjacencyMatrix[i][j] == false)
					{
						adjacencyMatrix[i][j] = true;
						break;
					}
				}
				else if (checkFullRare(adjacencyMatrix, IDs.size(), i) == true)
				{
					if (j != i && adjacencyMatrix[i][j] == false)
					{
						adjacencyMatrix[i][j] = true;
						break;
					}
				}
				else if (checkFullEpic(adjacencyMatrix, IDs.size(), i) == true)
				{
					if (j != i && adjacencyMatrix[i][j] == false)
					{
						adjacencyMatrix[i][j] = true;
						break;
					}
				}
			}
			adjacencyMatrix[i][j] = true;
			adjacencyMatrix[j][i] = true;
		}
		for (unsigned int i = 0; i < adjacencyMatrix[0].size(); i++)
		{
			for (unsigned int j = 0; j < adjacencyMatrix[0].size(); j++)
				cout << adjacencyMatrix[i][j] << '\t';
			cout << endl;
		}
		cout << endl;
	}
	return adjacencyMatrix;
}

bool FullNode::checkFull(vector<vector<bool>> m, int n, int i)
{
	for (int k = 0; k < n; k++)
	{
		if (m[i][k] == false && countConnections(m, k) <= 2)
		{
			return false;
		}
	}
	return true;
}

bool FullNode::checkFullRare(vector<vector<bool>> m, int n, int i)
{
	for (int k = 0; k < n; k++)
	{
		if (m[i][k] == false && countConnections(m, k) <= 3)
		{
			return false;
		}
	}
	return true;
}

bool FullNode::checkFullEpic(vector<vector<bool>> m, int n, int i)
{
	for (int k = 0; k < n; k++)
	{
		if (m[i][k] == false && countConnections(m, k) <= 4)
		{
			return false;
		}
	}
	return true;
}

int FullNode::countConnections(vector<vector<bool>> m, int i)
{
	int connections = 0;
	for (unsigned int n = 0; n < m.size(); n++)
	{
		connections += m[i][n];
	}
	return connections;
}

int FullNode::checkStrongConnections(vector<vector<bool>> matrix , int n)
{
	for (int i = 0; i < n; i++)
	{
		unsigned connections = 0;
		for (int j = 0; j < n; j++)
		{
			connections += matrix[i][j];
		}
		if (connections < 2)
		{
			return i;
		}
	}
	return -1;
}

void FullNode::traverse(vector<bool>& visited, vector<vector<bool>> adjacencyMatrix, unsigned qNodes, int u) {
	visited[u] = true;	//mark v as visited
	for (unsigned int v = 0; v < qNodes; v++) {
		if (adjacencyMatrix[u][v] && !visited[v])
			traverse(visited, adjacencyMatrix, qNodes, v);
	}
	return;
}
bool FullNode::isConnected(vector<vector<bool>> adjacencyMatrix, unsigned qNodes) {
	vector<bool> vis(qNodes, false);
	traverse(vis, adjacencyMatrix, qNodes);
	for (unsigned int i = 0; i < qNodes; i++) {
		if (!vis[i])	//if there is a node, not visited by traversal, graph is not connected
			return false;
	}
	return true;
}

void FullNode::cycleConnections() {
	vector<int> pendingErasing;

	if (servers.back()->acceptConnection()) {
		Server* newserver = new Server(socket.port);
		servers.push_back(newserver);
	}
	for (unsigned int i = 0; i < servers.size(); i++) {
		if (servers[i]->readRequest()) {
			string response = handleHttpRequest(servers[i]->getRequest());
			if (response.size() == 0) {
				servers[i]->sendResponse("404 Not Found", response);
			}
			else {
				servers[i]->sendResponse("200 OK", response);
			}
		}

		if (servers[i]->writeResponse()) {
			pendingErasing.push_back(i);
		}
	}
	int amountErased = pendingErasing.size();
	for (int i = amountErased-1; i >= 0; i--) {
		delete servers[pendingErasing[i]];
		servers.erase(servers.begin() + pendingErasing[i]);
	}
}

string FullNode::handleHttpRequest(string request) {
	string command;
	unsigned int nCommand = 0;
	string uri;			//Uri del mensaje (ej: 127.0.0.1:80/eda_coin/send_block/ )
	string method;		//Method: GET o POST
	string response = "";

	unsigned int i = 0;
	while(i < request.size()){
		if (request[i] != '\r' && request[i] != '\n') {
			command = "";
			while (request[i] != '\r' && request[i] != '\n') {
				command += request[i];
				i++;
				if (i == request.size()) break;
			}

			if (nCommand == 0) {	//First Line
				findURIandMethod(command, uri, method);
			}
			else if (method == "POST") {
				json j = json::parse(command, nullptr, false);

				if (j.is_discarded() == false) {
					response += handlePOSTcommand(j, uri);
				}
			}
			else if (method == "GET") {
				response += handleGETcommand(command, uri);
			}

			nCommand++;
		}

		i++;
	}

	return response;
}

void FullNode::findURIandMethod(string command, string& uri, string& method) {
	int uriIndex;
	int uriLength;
	size_t foundGet = command.find("GET ");	//Posicion del texto "GET "
	size_t foundPost = command.find("POST ");
	size_t foundHttp = command.find("HTTP");
	if (foundGet != string::npos) {
		uriIndex = foundGet + 4;
		method = "GET";
	}
	if (foundPost != string::npos) {
		uriIndex = foundPost + 5;
		method = "POST";
	}
	if (foundHttp != string::npos) {
		uriLength = foundHttp - uriIndex - 1;
		uri = command.substr(uriIndex, uriLength);
	}
	else {
		uri = "";
	}
}

string FullNode::handleGETcommand(string command, string uri) {
	if (uri == "/eda_coin/get_block_header/") {
		size_t foundId = command.find("block_id:");
		if (foundId != string::npos) {
			string idString = command.substr(foundId + 9);

			json j = generateBlockHeader(idString);
			if (j.empty() == false) {
				return j.dump();
			}
			else {
				return "{\"result\":false,\"errorCode\":unknown_id}";
			}
		}
	}
	else {
		return "";
	}
}

string FullNode::handlePOSTcommand(json j, string uri) {
	if (uri == "/eda_coin/send_block/") {
		blockChain.appendBlock(createBlock(j));
	}
	else if (uri == "/eda_coin/send_tx") {
		pendingTx.push_back(createTx(j));
	}
	else if (uri == "/eda_coin/send_filter") {
		string id = j["Id"];
		subscriptors.insert(id);
	}
	else {
		return "";
	}

	return "{\"result\":true,\"errorCode\":null}";
}