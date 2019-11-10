#include "FullNode.h"
#include "ServerRequest.h"
#include "ServerResponse.h"
#include <chrono>
#include <thread>

using namespace std;

FullNode::FullNode(SocketType socket, string ID, map<string, SocketType> neighbourNodes):
Node(socket, ID, neighbourNodes){
	pingStatus = false;
	timer = (rand() % 999) * 10 + 10;;
	state = IDLE;

	//Server
	IO_Handler = new boost::asio::io_service();
	//Creo el endpoint para recibir conexiones
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), getNodePort());
	serverSocket = new boost::asio::ip::tcp::socket(*IO_Handler);
	serverAcceptor = new boost::asio::ip::tcp::acceptor(*IO_Handler, ep);
	serverAcceptor->non_blocking(true);
	//serverSocket->non_blocking(true);
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
	case WAITING_LAYOUT:
		//epera mensaje NetwokLayout
		//responde HTTP OK
		//agrega vecinos
	case COLLECTING_NETWORK_MEMBERS:
		//envia PING a cada nodo
		//si NetworkNotReady agrega nodo a lista
		//si NetworkReady toma lista y conecta con la actual
	case NETWORK_CREATED:
		break;
		//
	}
}

void FullNode::acceptConnection() {
	boost::system::error_code error;
	serverAcceptor->accept(*serverSocket, error);
	if (error.value() != EWOULDBLOCK) {
		readingRequest = true;
		Request request = Request();
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
