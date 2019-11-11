#include "ServerRequest.h"

bool Request::isValid() {
    return !(this->host.empty() || this->uri.empty() || this->method.empty());
}

void Request::handleRequest(const char* buffer, unsigned int bufferSize) {
	string command;
	unsigned int nCommand = 0;

	for (unsigned int i = 0; i < bufferSize; i++) {
		command = "";
		while (buffer[i] != '\r' && buffer[i + 1] != '\n') {
			command += buffer[i];
			i++;
		}
		handleCommand(command, nCommand);
		nCommand++;
		i++; // Remove \n
	}
}

void Request::handleCommand(const string command, unsigned int nCommand) {
	if (nCommand == 0) {
		size_t foundGet = command.find("GET ");
		if (foundGet != string::npos) {
			method = "GET";
			size_t foundHttp = command.find("HTTP");
			if (foundHttp != string::npos) {
				uri = command.substr(foundGet + 5, foundHttp - 5);
			}
		}
	}
	else {
		size_t foundHost = command.find("Host: ");
		if (foundHost != string::npos) {
			host = command.substr(foundHost + 6);
		}
	}
}