#include "FullNode.h"
#include "JsonController.h"
#include <chrono>
#include <thread>

using namespace std;
using namespace CryptoPP;

FullNode::FullNode(SocketType socket, string ID, map<string, SocketType> neighbourNodes)
{
	this->socket = socket;
	this->ID = ID;
	this->neighbourNodes = neighbourNodes;

	timer = al_create_timer(((rand() % 999) * 10.0 + 10.0) / 1000.0);
	state = IDLE;

	Server* newserver = new Server(socket.port);
	servers.push_back(newserver);

	blockChain.attach(blockViewer);

	AutoSeededRandomPool autoSeededRandomPool;
	privateKey.Initialize(autoSeededRandomPool, ASN1::secp256k1());
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
		if (!al_get_timer_started(timer))
			al_start_timer(timer);

		if (al_get_timer_count(timer) >= 1) {
			al_stop_timer(timer);
			state = COLLECTING_NETWORK_MEMBERS;

			cout << ID << " is the new team leader!" << endl;
		}
		break;
	case COLLECTING_NETWORK_MEMBERS:
		if (sendNextPing() == 0) {
			json j = p2pAlgorithm(onlineNodes);
			networkLayout = j.dump();

			prevNodeIt = onlineNodes.begin();
			nextNodeIt = onlineNodes.begin(); //Empiezo mandando layout al primer nodo

			cout << "Network Layout: " << networkLayout << endl;

			state = SENDING_LAYOUTS;
		}
		break;
	case SENDING_LAYOUTS:
		if (sendNextLayout() == 0) {
			//No se que onda con los network_ready
			state = NETWORK_CREATED;
		}
	}
}

void FullNode::setManifestPath(string p) {
	manifestPath = p;
	manifestNodes = getNodesFromManifest();
	prevNodeIt = manifestNodes.begin();
	nextNodeIt = manifestNodes.begin();
	nextNodeIt++;	//Empiezo pingueando el segundo nodo
}

void FullNode::sendPing(SocketType s) {
	httpPost(s.IP, s.port, "/path/PING", "Host:localhost", 5);	//Timeout 5ms
}

int FullNode::sendNextPing() {
	//Enviar proximo ping
	if (manifestNodes.size() > 0) {
		if (isPerformingFetch() == false && getNewResponse() == false) {
			if (nextNodeIt->first != ID) {
				sendPing(nextNodeIt->second);

				prevNodeIt = nextNodeIt;
				++nextNodeIt;
				if (nextNodeIt == manifestNodes.end()) nextNodeIt = manifestNodes.begin();

				//Debug
				cout << "Ping al vecino: " << prevNodeIt->first << endl;
			}
			else {
				prevNodeIt = nextNodeIt;
				++nextNodeIt;
				if (nextNodeIt == manifestNodes.end()) nextNodeIt = manifestNodes.begin();

				cout << prevNodeIt->first << " is the leader, he needs no PING" << endl;
				string id = prevNodeIt->first;
				SocketType sock = prevNodeIt->second;
				onlineNodes.insert((std::pair<string, SocketType>(id, sock)));
				manifestNodes.erase(prevNodeIt);
			}
		}
	}

	return manifestNodes.size();
}

int FullNode::sendNextLayout() {
	//Enviar proximo layout
	if (onlineNodes.size() > 0) {
		if (isPerformingFetch() == false && getNewResponse() == false) {
			if (nextNodeIt->first != ID) {
				sendLayout(nextNodeIt->second);

				prevNodeIt = nextNodeIt;
				++nextNodeIt;
				if (nextNodeIt == onlineNodes.end()) nextNodeIt = onlineNodes.begin();

				//Debug
				cout << "Layout al vecino: " << prevNodeIt->first << endl;
			}
			else {
				prevNodeIt = nextNodeIt;
				++nextNodeIt;
				if (nextNodeIt == onlineNodes.end()) nextNodeIt = onlineNodes.begin();

				cout << prevNodeIt->first << " is the leader, he needs no LAYOUT" << endl;
				string id = prevNodeIt->first;
				SocketType sock = prevNodeIt->second;
				onlineNodes.erase(prevNodeIt);

				addNeighboursFromLayout(networkLayout);
			}
		}
	}

	return onlineNodes.size();
}

void FullNode::sendLayout(SocketType s) {
	httpPost(s.IP, s.port, "/path/NETWORK_LAYOUT", "Host:localhost", 5);	//Timeout 5ms
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
		if (getHttpMethod() == "POST") {
			if (getHttpURI() == "/path/PING") {
				string r = getResponse();
				json response = json::parse(r, nullptr, false);
				if (response.is_discarded() == false && response.is_object()) {
					if (response["status"] == false) {	//Network_not_ready
						cout << prevNodeIt->first << " responded to PING" << endl;
						string id = prevNodeIt->first;
						SocketType sock = prevNodeIt->second;
						onlineNodes.insert((std::pair<string, SocketType>(id, sock)));
						manifestNodes.erase(prevNodeIt);
					}
				}
			}
		}
		else if (getHttpMethod() == "GET") {
			//json response(n->getResponse());

			//Received Block Header, do something
		}
		setNewResponse(false);
	}
}

string FullNode::respondToCommands(vector<string> commands, string uri, string method, string ip, int port) {
	string response = "";

	switch (state) {
		case IDLE:
			if (uri == "/path/PING") {
				response = "{\"status\":false}";	//Esto representa Network Not Ready
				state = WAITING_LAYOUT;
			}
			break;
		case WAITING_LAYOUT:
			if (uri == "/path/NETWORK_LAYOUT") {
				for (unsigned int i = 0; i < commands.size(); i++) {
					json j = json::parse(commands[i], nullptr, false);

					if (j.is_discarded() == false) {
						cout << ID<<": Received Layout!" << endl;
						addNeighboursFromLayout(j.dump());
						response = "{\"status\":true}";	//Esto representa Network Ready
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
				return "{\"result\":false,\"errorCode\":\"unknown_id\"}";
			}
		}
	}
	else if (uri == "/test/") {
		return "{\"result\":false,\"errorCode\":\"test\"}";
	}
	else {
		return "";
	}

	return "{\"result\":true,\"errorCode\":null}";
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
	else if (uri == "/test/") {
		"{\"result\":false,\"errorCode\":\"test\"}";
	}
	else {
		return "";
	}

	return "{\"result\":true,\"errorCode\":null}";
}


//fase 4

bool FullNode:: validateTx(Transaction tx)
{
	bool c1 = false, c2 = false, c3 = false;
	//ECDSA<ECP, SHA256>::PublicKey publicKey;
	//privateKey.MakePublicKey(publicKey);
	
	if (tx.txid != generateTxid(tx))
	{
		c1 = true;
	}

	vector<Transaction>::iterator it; 
	for (it = utxo.begin(); it != utxo.end(); it++)
	{
		if (it->txid == tx.txid)
		{
			c2 == true;
		}
	}


	//checkeo que las salidas coinciden con las entradas
	unsigned int amount1 = 0, amount2 = 0;
	for (int i = 0; i < tx.vout.size(); i++)
	{
		amount1 += tx.vout[i].amount;
	}
	//recorro todas las entradas
	for (int i = 0; i < tx.vin.size(); i++)
	{
		//busco el bloque y guardo en index en bidx
		int bidx;
		for ( bidx = 0; bidx < blockChain.getBlockchainSize(); bidx++)
		{
			if (tx.vin[i].blockid == blockChain.getBlockId(bidx))
				break;
		}
		//recorro todas las transacciones de ese bloque
		for (int txidx = 0; txidx < blockChain.getBlockTransactionNumber(bidx); txidx++)
		{
			Transaction txI = blockChain.getTxInBlock(bidx, txidx);
			//recorro todo el bloque vout de esta transaccion
			for (int o = 0; o < txI.nTxout; o++)
			{
				if (txI.vout[o].publicid == tx.publicid)
					amount2 += txI.vout[o].amount;
			}
		}
	}
	if (amount1 == amount2)
	{
		c3 = true;
	}

	return c1 && c2 && c3;
}

bool FullNode:: validateBlock(Block block, int challenge)
{
	bool val = true;
	string b = hex_str_to_bin_str(block.blockid);
	for(int i = 0; i < challenge; i++)
	{
		if (b[i] != '0')
			val = false;
	}
	int i;
	for ( i = 0; i < blockChain.getBlockchainSize(); i++)
	{
		if (blockChain.getBlockId(i) == block.blockid)
			break;
	}
	if (i != 0 && (blockChain.getBlockId(i - 1) == block.previousblockid) )
	{
		val == false;
	}
	for (int j = 0; j < blockChain.getBlockTransactionNumber(i); j++)
	{
		if (!validateTx(blockChain.getTxInBlock(i, j)))
		{
			val = false;
		}
	}
	return val;
}

string FullNode:: generateBlockID(Block block)
{
	//generate blockid
	string s;
	s += block.previousblockid;
	s += block.height;
	s += block.merkleroot;
	s += block.nonce;

	CryptoPP::SHA256 hash;
	byte digest[CryptoPP::SHA256::DIGESTSIZE];
	hash.CalculateDigest(digest, (byte*)s.c_str(), s.length());

	CryptoPP::HexEncoder encoder;
	std::string blockid;
	encoder.Attach(new CryptoPP::StringSink(blockid));
	encoder.Put(digest, sizeof(digest));
	encoder.MessageEnd();

	return blockid;
}

const char* FullNode:: hex_char_to_bin(char c)
{
	// TODO handle default / error
	switch (toupper(c))
	{
	case '0': return "0000";
	case '1': return "0001";
	case '2': return "0010";
	case '3': return "0011";
	case '4': return "0100";
	case '5': return "0101";
	case '6': return "0110";
	case '7': return "0111";
	case '8': return "1000";
	case '9': return "1001";
	case 'A': return "1010";
	case 'B': return "1011";
	case 'C': return "1100";
	case 'D': return "1101";
	case 'E': return "1110";
	case 'F': return "1111";
	default: return "0000";
	}
}

string FullNode:: hex_str_to_bin_str(const std::string& hex)
{
	// TODO use a loop from <algorithm> or smth
	string bin;
	for (unsigned i = 0; i != hex.length(); ++i)
		bin += hex_char_to_bin(hex[i]);
	return bin;
}

void FullNode::addNeighboursFromLayout(string layout)
{
	int begin = layout.rfind(ID);
	if (begin == string::npos)
	{
		cout << "Error:" << ID << "not found in layout" << endl;
		return;
	}
	int target1 = 0, target2 = 0;
	char node1[64];
	char node2[64];
	while (target1 != string::npos && target2 != string::npos)
	{
		target1 = layout.find("target1", target1);
		target2 = layout.find("target2", target2);
		layout.copy(node1, 64, target1 + 2);
		layout.copy(node2, 64, target2 + 2);
		if (node1 != ID && node2 == ID) appendNeighbourNode(node1, { "", 0 }); //falta el SOCKET
		else if (node2 != ID && node1 == ID) appendNeighbourNode(node2, { "", 0 });
	}
}
