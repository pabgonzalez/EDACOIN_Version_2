#include "Server.h"
#include <time.h>

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
	clientIP = "";
	clientPort = 0;
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
			readingRequest = false;
			requestReady = true;

			clientIP = serverSocket->remote_endpoint().address().to_string();
			clientPort = serverSocket->remote_endpoint().port();
			parseHttpRequest(receivedMessage);

			cout << "Se recibio el siguiente mensaje:" << endl << receivedMessage << endl;
			cout << "Fue emitido desde el IP: " << clientIP << " Puerto: " << clientPort << endl;

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

string Server::getClientIP() {
	if (requestReady) {
		return clientIP;
	}
	else {
		return "";
	}
}

int Server::getClientPort() {
	if (requestReady) {
		return clientPort;
	}
	else {
		return 0;
	}
}

void Server::sendResponse(string status, string content) {
	response = "HTTP/1.1 ";
	response += status;
	response += "\r\nDate: ";
	response += getCurrentDateString();
	if (status != "404 Not Found") {
		response += "\r\nLocation: ";
		response += HOST_NAME;
		//response += "/";
		response += getURI();
	}
	response += "\r\nCache-Control: ";
	response += "max-age=30";
	response += "\r\nExpires: ";
	response += getCurrentDateString(EXPIRE_TIME);;
	response += "\r\nContent-Length: ";
	response += to_string(content.size());
	response += "\r\nConnection: close";
	response += "\r\nContent-Type: ";
	response += "text/html; charset=iso-8859-1";
	response += "\r\n\r\n";
	response += content;

	writingResponse = true;

	/*
	response = "HTTP/1.1 ";
	response += status + '\n';
	response += content;
	writingResponse = true;
	*/
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

char* Server::getCurrentDateString(int delay) {
	string dateString;
	const char* days[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
	const char* months[] = { "Jan", "Feb", "Mar", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

	time_t rawtime;
	struct tm* ptm;
	time(&rawtime);
	ptm = gmtime(&rawtime);

	char timeChar[50];

	int time = ptm->tm_hour * 3600 + ptm->tm_min * 60 + ptm->tm_sec + delay;
	ptm->tm_hour = time / 3600;
	ptm->tm_min = (time - ptm->tm_hour * 3600) / 60;
	ptm->tm_sec = time % 60;

	dateString = days[ptm->tm_wday];
	dateString += ", ";
	dateString += to_string(ptm->tm_mday);
	dateString += " ";
	dateString += months[ptm->tm_mon - 1];
	dateString += " ";
	dateString += to_string(1900 + ptm->tm_year);
	dateString += " ";
	snprintf(timeChar, 50, "%02i", ptm->tm_hour);
	dateString += timeChar;
	dateString += ":";
	sprintf(timeChar, "%02i", ptm->tm_min);
	dateString += timeChar;
	dateString += ":";
	sprintf(timeChar, "%02i", ptm->tm_sec);
	dateString += timeChar;
	dateString += " GMT";

	char* date = new char[dateString.length() + 1];
	strcpy(date, dateString.c_str());

	return date;
}
