#include "Server.h"

Server::Server(unsigned int port) {
	//cout << "Se creo un servidor" << endl;

	//Server
	IO_Handler = new boost::asio::io_service();
	//Creo el endpoint para recibir conexiones
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), port);
	serverAcceptor = new boost::asio::ip::tcp::acceptor(*IO_Handler, ep);
	serverAcceptor->non_blocking(true);

	serverSocket = new boost::asio::ip::tcp::socket(*IO_Handler);
	readingRequest = false;
	requestReady = false;
	writingResponse = false;
	response = "";
	uri = "";
	method = "";
	commands.clear();
}

Server::~Server() {
	//cout << "Se destruyo un servidor" << endl;
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
		requestReady = false;

		return true;
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

			readingRequest = false;
			requestReady = true;

			parseHttpRequest(receivedMessage);

			return true;
		}
	}
	return false;
}

bool Server::writeResponse() {
	if (writingResponse) {
		boost::system::error_code error;
		size_t len;
		len = serverSocket->write_some(boost::asio::buffer(response.c_str(), strlen(response.c_str())), error);

		if (error.value() != WSAEWOULDBLOCK) {
			cout << "Enviando el siguiente mensaje:" << endl << response << endl;
			writingResponse = false;
			return true;
		}
	}
	return false;
}

string Server::getRequest() {
	if (requestReady) {
		return receivedMessage;
	}
	else {
		return "";
	}
}

void Server::sendResponse(string status, string content) {
	response = "HTTP/1.1 ";
	response += status + '\n';
	response += content;
	writingResponse = true;
}

void Server::parseHttpRequest(string request) {
	commands.clear();
	string command;
	unsigned int nCommand = 0;

	unsigned int i = 0;
	while (i < request.size()) {
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
			else {
				commands.push_back(command);
			}
			nCommand++;
		}
		i++;
	}
}

void Server::findURIandMethod(string command, string& uri, string& method) {
	int uriIndex = 0;
	int uriLength = 0;
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