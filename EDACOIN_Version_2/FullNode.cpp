#include "FullNode.h"
#include "ServerRequest.h"
#include "ServerResponse.h"
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
	//Server
	IO_Handler = new boost::asio::io_service();
	//Creo el endpoint para recibir conexiones
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), getNodePort());
	serverAcceptor = new boost::asio::ip::tcp::acceptor(*IO_Handler, ep);
	serverAcceptor->non_blocking(true);
	serverAcceptor->listen(256);

	serverSocket = new boost::asio::ip::tcp::socket(*IO_Handler);

	readingRequest = false;
	writingResponse = false;
	request = Request();
	response = Response(request);
}

FullNode::~FullNode() {
	serverAcceptor->close();
	serverSocket->close();
	delete serverAcceptor;
	delete serverSocket;
	delete IO_Handler;
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
	return p2pRecursive(IDs);
}

vector<vector<bool>> FullNode::p2pRecursive(vector<string>& IDs)
{
	static vector<vector<bool>> adjacencyMatrix(IDs.size(), vector<bool>(IDs.size(), 0));
	while (checkStrongConnections(adjacencyMatrix, IDs.size()) != -1 )
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
		for (int i = 0; i < adjacencyMatrix[0].size(); i++)
		{
			for (int j = 0; j < adjacencyMatrix[0].size(); j++)
				cout << adjacencyMatrix[i][j] << '\t';
			cout << endl;
		}
		cout << endl;
	}
	if (isConnected(adjacencyMatrix, IDs.size()) == false)
	{
		cout << "No conexo" << endl;
	}

	//if (isConnected(adjacencyMatrix, IDs.size()) )
	//{
	//
	//	if ( checkStrongConnections(adjacencyMatrix, IDs.size())) == -1)
	//		return adjacencyMatrix;
		//else
		//{
		//	unsigned newNeighbour = disconnected;
		//	while (newNeighbour == disconnected || adjacencyMatrix[disconnected][newNeighbour] == true)
		//	{
		//		newNeighbour = rand() % IDs.size();
		//	}
		//	adjacencyMatrix[disconnected][newNeighbour] = true;
		//	adjacencyMatrix[newNeighbour][disconnected] = true;
		//}
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
	for (int n = 0; n < m.size(); n++)
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

//https://www.tutorialspoint.com/cplusplus-program-to-check-the-connectivity-of-undirected-graph-using-dfs
void FullNode::traverse(vector<bool>& visited, vector<vector<bool>> adjacencyMatrix, unsigned qNodes, int u) {
	visited[u] = true;	//mark v as visited
	for (int v = 0; v < qNodes; v++) {
		if (adjacencyMatrix[u][v] && !visited[v])
			traverse(visited, adjacencyMatrix, qNodes, v);
	}
	return;
}
bool FullNode::isConnected(vector<vector<bool>> adjacencyMatrix, unsigned qNodes) {
	vector<bool> vis(qNodes, false);
	traverse(vis, adjacencyMatrix, qNodes);
	for (int i = 0; i < qNodes; i++) {
		if (!vis[i])	//if there is a node, not visited by traversal, graph is not connected
			return false;
	}
	return true;
}

void FullNode::acceptConnection() {
	//auto callback = bind(&FullNode::acceptHandler, this, _1);
	//serverAcceptor->async_accept(*serverSocket, callback);
	try {
		serverAcceptor->accept(*serverSocket);
	}
	catch(boost::system::system_error e){

	}
}

void FullNode::writeResponse() {
	if (writingResponse) {
		auto callback = bind(&FullNode::writeHandler, this, _1, _2);
		serverSocket->async_write_some(boost::asio::buffer(response.toString(), strlen(response.toString())), callback);
	}
}

void FullNode::readRequest() {
	if (readingRequest) {
		auto callback = bind(&FullNode::readHandler, this, _1, _2);
		serverSocket->async_read_some(boost::asio::buffer(buf), callback);
	}
}

void FullNode::readHandler(const boost::system::error_code& error, std::size_t len) {
	cout << getNodeID() << " received the following:" << buf << endl;

	if (len < 1) {
		request.handleRequest(buf, len);
		memset(buf, 0, 512 - 1);

		if (request.isValid()) {
			readingRequest = false;
			writingResponse = true;
			response = Response(request);
		}
		else {
			readingRequest = false;
			writingResponse = false;
		}
	}
}

void FullNode::writeHandler(const boost::system::error_code& error, std::size_t len) {
	if (len < 1) {
		writingResponse = false;
	}
}

void FullNode::acceptHandler(const boost::system::error_code& error)
{
	cout << error.value() << endl;
	if (!error)
	{
		cout << getNodeID() << " accepted a connection" << endl;

		serverSocket->non_blocking(true);
		readingRequest = true;
		Request request = Request();
	}
}
