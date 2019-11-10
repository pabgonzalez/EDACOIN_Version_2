#include "ServerResponse.h"
#include <time.h>
#include <string>
#include <fstream>

Response::Response() {
	cache_control = "max-age=30";
	content_type = "text/html; charset=iso-8859-1";
	status = "200 OK";
	location = "";
	content_length = 0;
	html_content = "";
}

Response::Response(Request &request) {
    cache_control = "max-age=30";
    content_type = "text/html; charset=iso-8859-1";

    try {
		FILE* fp;
		fp = fopen(request.getUri().c_str(), "r");
		if (fp == NULL) throw ifstream::failure("Error abriendo archivo");

        status = "200 OK";
        location = request.getUri();

        char c;
        content_length = 0;
		c = fgetc(fp);
        while (!feof(fp)){
            html_content += c;
            content_length++;
			c = fgetc(fp);
        }
    }
    catch(ifstream::failure e) {
        //404
        status = "404 Not Found";
        content_length = 0;
    }
}

char *Response::toString() {
    string responseString;
    responseString = "HTTP/1.1 ";
    responseString += status;
    
    responseString += html_content;

    char * response = new char[responseString.length() + 1];
    strcpy(response, responseString.c_str());
    return response;
}

