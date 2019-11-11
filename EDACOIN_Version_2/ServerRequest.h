#pragma once

#include <iostream>
#include <string>

using namespace std;

class Request {
public:
    void setMethod(string method) {this->method=method;}
    void setUri(string uri) {this->uri=uri;}
    void setHost(string host) {this->host=host;}
	string getMethod() {return this->method;}
	string getUri() {return this->uri;}
	string getHost() {return this->host;}
	void handleRequest(const char* buffer, unsigned int bufferSize); // Check if the request is valid and save data
    bool isValid();
private:
	void handleCommand(const string command, unsigned int nCommand);

	string method;
	string uri;
	string host;
};