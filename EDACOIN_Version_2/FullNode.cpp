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


	timer = al_create_timer(((rand() % 999) * 10.0 + 10.0) / 1000.0);
	//timer = (rand() % 999) * 10 + 10;
	state = IDLE;

	Server* newserver = new Server(socket.port);
	servers.push_back(newserver);

	blockChain.attach(blockViewer);
}

FullNode::~FullNode() {
	for (unsigned int i = 0; i < servers.size(); i++) {
		delete servers[i];
	}
	al_destroy_timer(timer);
}

void FullNode::p2pFSM() {
	switch (state) {
	case IDLE:
		if(!al_get_timer_started(timer))
			al_start_timer(timer);

		if (al_get_timer_count(timer) >= 1) {
			al_stop_timer(timer);
			state = COLLECTING_NETWORK_MEMBERS;
		}
		break;
	case COLLECTING_NETWORK_MEMBERS:
		if (sendNextPing() == 0) {
			//No se que onda con los network_ready
			state = NETWORK_CREATED;
		}
	}
}

void FullNode::setManifestPath(string p) {
	manifestPath = p;
	manifestNodes = getNodesFromManifest();
	pingNodeIt = manifestNodes.begin();
}

void FullNode::sendPing(SocketType s) {
	httpPost(s.IP, s.port, "/path/PING", "Host:localhost", 5);	//Timeout 5ms
}

int FullNode::sendNextPing() {
	//Enviar proximo ping
	if (isPerformingFetch() == false && getNewResponse() == false) {
		if (pingNodeIt->first != ID) {
			sendPing(pingNodeIt->second);

			//Debug
			cout << "Ping al vecino: " << pingNodeIt->first << endl;
		}
		++pingNodeIt;
		if (pingNodeIt == manifestNodes.end()) pingNodeIt = manifestNodes.begin();
	}

	return manifestNodes.size();
}

void FullNode::sendMerkleBlock(string nodeid, string blockid, string txid) {
	if (isNeighbour(nodeid)) {
		json j = generateMerkleBlock(blockid, txid);
		string aux = j;

		httpPost(nodeid, "/eda_coin/send_merkle_block/", aux);
	}
}

void FullNode::sendBlock(string nodeid, string blockid) {
	if (isNeighbour(nodeid)) {
		json j = generateBlockJson(blockid);
		string aux = j;

		httpPost(nodeid, "/eda_coin/send_block/", aux);
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

json FullNode::p2pAlgorithm(map<string, SocketType> Nodes)
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

	//write Layout
	json j;
	for (int i = 0; i < adjacencyMatrix.front().size(); i++)
	{
		j["nodes"] += IDs[i];
		for (int k = i; k < adjacencyMatrix.front().size(); k++)
		{
			if (adjacencyMatrix[i][k] == true)
			{
				j["edges"] += { {"target1", IDs[i]}, { "target2", IDs[k] }};
			}
		}
	}

	return j;
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

//Tx Flood
void FullNode::floodTransaction(Transaction tx, string ip, int port)
{
	map<string, SocketType>::iterator it;
	for (it = neighbourNodes.begin(); it != neighbourNodes.end(); ++it)
	{
		if (it->second.IP != ip || it->second.port != port) //Si no se trata del emisor original
		{
			sendTx(it->first, tx);
		}
	}
}

//Block Flood
void FullNode::floodBlock(Block b, string ip, int port)
{
	map<string, SocketType>::iterator it;
	for (it = neighbourNodes.begin(); it != neighbourNodes.end(); ++it)
	{
		if (it->second.IP != ip || it->second.port != port) //Si no se trata del emisor original
		{
			sendBlock(it->first, b.blockid);
		}
	}
}

void FullNode::cycleConnections() {
	vector<int> pendingErasing;

	if (servers.back()->acceptConnection()) {
		Server* newserver = new Server(socket.port);
		servers.push_back(newserver);
	}
	for (unsigned int i = 0; i < servers.size(); i++) {
		if (servers[i]->readRequest()) {
			string uri = servers[i]->getURI();
			string method = servers[i]->getMethod();
			string ip = servers[i]->getClientIP();
			int port = servers[i]->getClientPort();
			string response = respondToCommands(servers[i]->getCommands(), uri, method, ip, port);

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

void FullNode::handleResponse() {
	if (getNewResponse()) {
		cout << "Recibi la siguiente respuesta:" << endl;
		cout << getResponse() << endl;
		/*if (getHttpMethod() == "POST") {
			if (getHttpURI() == "/path/PING") {
				json response(getResponse());
			}
		}
		else if (getHttpMethod() == "GET") {
			json response(n->getResponse());

			//Received Block Header, do something
		}*/
		setNewResponse(false);
	}
}

string FullNode::respondToCommands(vector<string> commands, string uri, string method, string ip, int port) {
	string response = "";

	switch (state) {
		case IDLE:
			if (uri == "/path/PING") {
				response = "{ \"status\": false }";	//Esto representa Network Not Ready
				state = WAITING_LAYOUT;
			}
			break;
		case WAITING_LAYOUT:
			if (uri == "/path/NETWORK_LAYOUT") {
				for (unsigned int i = 0; i < commands.size(); i++) {
					json j = json::parse(commands[i], nullptr, false);

					if (j.is_discarded() == false) {
						//response += PARSEAR LAYOUT
					}
				}
			}
			break;
		//case COLLECTING_NETWORK_MEMBERS:
			//	//si NetworkNotReady agrega nodo a lista
			//	//si NetworkReady toma lista y conecta con la actual
		//	break;
		case NETWORK_CREATED:
			for (unsigned int i = 0; i < commands.size(); i++) {
				if (method == "POST") {
					json j = json::parse(commands[i], nullptr, false);

					if (j.is_discarded() == false) {
						response += handlePOSTcommand(j, uri, ip, port);
					}
				}
				else if (method == "GET") {
					response += handleGETcommand(commands[i], uri, ip, port);
				}
			}
			break;
	}

	return response;
}

string FullNode::handleGETcommand(string command, string uri, string ip, int port) {
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

string FullNode::handlePOSTcommand(json j, string uri, string ip, int port) {
	if (uri == "/eda_coin/send_block/") {
		bool blockIsNew = true;
		Block b = createBlock(j);
		//Empiezo por el final, probablemente el bloque fue recientemente agregado
		for (int i = blockChain.getBlockchainSize() - 1; i >= 0; i--) {
			if (b.blockid == blockChain.getBlockId(i)) {
				blockIsNew = false;
				break;
			}
		}
		if (blockIsNew) {
			blockChain.appendBlock(b);
			floodBlock(b, ip, port);
		}
	}
	else if (uri == "/eda_coin/send_tx") {
		bool txIsNew = true;
		Transaction tx = createTx(j);
		for (unsigned int i = 0; i < pendingTx.size(); i++) {
			if (tx.txid == pendingTx[i].txid) {
				txIsNew = false;
				break;
			}
		}
		if (txIsNew) {
			pendingTx.push_back(tx);
			floodTransaction(tx, ip, port);
		}
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