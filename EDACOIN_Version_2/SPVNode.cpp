#include "SPVNode.h"
#include "JsonController.h"

SPVNode::SPVNode(SocketType socket, string ID, map<string, SocketType> neighbourNodes) :
	Node(socket, ID, neighbourNodes) {

	server = new Server(socket.port);
}

SPVNode::~SPVNode() {
	delete server;
}

void SPVNode::sendFilter(string nodeid) {
	if (isNeighbour(nodeid)) {
		json j;
		j["Id"] = ID;
		string aux = j;
		httpPost(nodeid, "/eda_coin/send_filter/", j);
	}
}

void SPVNode::getBlockHeader(string nodeid, string blockid) {
	if (isNeighbour(nodeid)) {
		httpGet(nodeid, "/eda_coin/get_block_header/", "block_id:" + blockid);
	}
}

json SPVNode::generateFilter() {
	json j;
	j["Id"] = ID;
	return j;
}

void SPVNode::cycleConnections() {
	server->acceptConnection();
	if (server->readRequest()) {
		string uri = server->getURI();
		string method = server->getMethod();
		string response = respondToCommands(server->getCommands(), uri, method);
		
		if (response.size() == 0) {
			server->sendResponse("404 Not Found", response);
		}
		else {
			server->sendResponse("200 OK", response);
		}
	}
	server->writeResponse();
}

string SPVNode::respondToCommands(vector<string> commands, string uri, string method) {
	string response = "";

	for (int i = 0; i < commands.size(); i++) {
		if (method == "POST") {
			json j = json::parse(commands[i], nullptr, false);

			if (j.is_discarded() == false) {
				response += handlePOSTcommand(j, uri);
			}
		}
		//else if (method == "GET") {
		//	response += handleGETcommand(commands[i], uri);
		//}
	}

	return response;
}

string SPVNode::handlePOSTcommand(json j, string uri) {
	if (uri == "/eda_coin/send_merkle_block/") {
		//Hacer algo con el merkle block ?
	}else{
		return "";
	}

	return "{\"result\":true,\"errorCode\":null}";
}