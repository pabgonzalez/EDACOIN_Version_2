#include "ServerResponse.h"
#include <time.h>
#include <string>
#include <fstream>

Response::Response(Request &request) {
    date = getCurrentDateString();
    expires = getCurrentDateString(EXPIRE_TIME);

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
    responseString += "\r\nDate: ";
    responseString += date;
    responseString += "\r\nLocation: 127.0.0.1/";
    responseString += location;
    responseString += "\r\nCache-Control: ";
    responseString += cache_control;
    responseString += "\r\nExpires: ";
    responseString += expires;
    responseString += "\r\nContent-Length: ";
    responseString += to_string(content_length);
    responseString += "\r\nConnection: close";
    responseString += "\r\nContent-Type: ";
    responseString += content_type;
    responseString += "\r\n\r\n";
    responseString += html_content;

    char * response = new char[responseString.length() + 1];
    strcpy(response, responseString.c_str());
    return response;
}

char *Response::getCurrentDateString(int delay) {
    string dateString;
    const char * days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    const char * months[] = {"Jan", "Feb", "Mar", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    time_t rawtime;
    struct tm * ptm;
    time ( &rawtime );
    ptm = gmtime ( &rawtime );

    char timeChar[100];

    int time = ptm->tm_hour*3600 + ptm->tm_min*60 + ptm->tm_sec + delay;
    ptm->tm_hour = time/3600;
    ptm->tm_min = (time-ptm->tm_hour*3600)/60;
    ptm->tm_sec = time%60;

    dateString = days[ptm->tm_wday];
    dateString += ", ";
    dateString += to_string(ptm->tm_mday);
    dateString += " ";
    dateString += months[ptm->tm_mon-1];
    dateString += " ";
    dateString += to_string(1900+ptm->tm_year);
    dateString += " ";
    sprintf(timeChar, "%02i", ptm->tm_hour);
    dateString +=  timeChar;
    dateString += ":";
    sprintf(timeChar, "%02i", ptm->tm_min);
    dateString +=  timeChar;
    dateString += ":";
    sprintf(timeChar, "%02i", ptm->tm_sec);
    dateString +=  timeChar;
    dateString += " GMT";

    char * date = new char[dateString.length() + 1];
    strcpy(date, dateString.c_str());

    return date;
}

