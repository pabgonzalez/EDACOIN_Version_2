#pragma once

#include "ServerRequest.h"
#include <iostream>

#define EXPIRE_TIME     30

using namespace std;

class Response {
public:
    Response(Request &request);
    char * toString();
private:
    string status;
	string date;
	string location;
	string cache_control;
	string expires;
	string content_type;
    int content_length;
    string html_content;

    char * getCurrentDateString(int delay=0);
};