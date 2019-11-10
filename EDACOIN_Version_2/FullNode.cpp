#include "FullNode.h"
#include "ServerRequest.h"
#include "ServerResponse.h"
#include <chrono>
#include <thread>

using namespace std;

FullNode::FullNode() {
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
	serverSocket->non_blocking(true);
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
		//
	}
}

boost::system::error_code FullNode::acceptConnection() {
	boost::system::error_code error;
	serverAcceptor->accept(*serverSocket, error);
	if (error.value() != EWOULDBLOCK) {
		readingRequest = true;
		Request request = Request();
	}

	return error;
}

boost::system::error_code FullNode::writeResponse() {
	boost::system::error_code error;
	if (writingResponse) {
		size_t len = serverSocket->write_some(boost::asio::buffer(response.toString(), strlen(response.toString())), error);
		if (error.value() != WSAEWOULDBLOCK) {
			writingResponse = false;
		}
	}
	return error;
}

boost::system::error_code FullNode::readRequest() {
	boost::system::error_code error;
	if (readingRequest) {
		size_t len = serverSocket->read_some(boost::asio::buffer(buf), error);
		if (error.value() != WSAEWOULDBLOCK) {
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
	return error;
}

