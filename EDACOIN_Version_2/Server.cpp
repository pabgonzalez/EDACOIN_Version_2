#include "Server.h"

Server::Server(unsigned int port) {
	cout << "Se creo un servidor" << endl;

	//Server
	IO_Handler = new boost::asio::io_service();
	//Creo el endpoint para recibir conexiones
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), port);
	serverAcceptor = new boost::asio::ip::tcp::acceptor(*IO_Handler, ep);
	serverAcceptor->non_blocking(true);

	serverSocket = new boost::asio::ip::tcp::socket(*IO_Handler);
	readingRequest = false;
	writingResponse = false;
	request = Request();
	response = Response();
}

Server::~Server() {
	cout << "Se destruyo un servidor" << endl;
	serverAcceptor->close();
	serverSocket->close();
	delete serverAcceptor;
	delete serverSocket;
	delete IO_Handler;
}


bool Server::acceptConnection() {
	boost::system::error_code error;
	serverAcceptor->accept(*serverSocket, error);

	if (error.value() != WSAEWOULDBLOCK) {
		cout << "Se acepto una conexion!" << endl;

		//serverSocket->non_blocking(true);
		readingRequest = true;
		Request request = Request();

		return true;
	}
	return false;
}

bool Server::writeResponse() {
	if (writingResponse) {
		boost::system::error_code error;
		size_t len;
		len = serverSocket->write_some(boost::asio::buffer(response.toString(), strlen(response.toString())), error);

		if (error.value() != WSAEWOULDBLOCK) {
			cout << "Enviando el siguiente mensaje:" << endl << response.toString() << endl;
			writingResponse = false;
			return true;
		}
	}
	return false;
}

bool Server::readRequest() {
	if (readingRequest) {
		char buf[512] = {};
		boost::system::error_code error;
		size_t len = 0;
		len = serverSocket->read_some(boost::asio::buffer(buf), error);
		receivedMessage += buf;
		if (error.value() != WSAEWOULDBLOCK) {
			cout << "Se recibio el siguiente mensaje:" << endl << receivedMessage << endl;

			request.handleRequest(receivedMessage.c_str(), receivedMessage.size());

			if (request.isValid()) {
				readingRequest = false;
				writingResponse = true;
				response = Response(request);
			}
			else {
				readingRequest = false;
				writingResponse = false;
			}
			return true;
		}
	}
	return false;
}