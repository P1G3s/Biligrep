#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "header.h"
#include "spawner.h"

#define PROTOCOL "HTTP/1.1"
#define REQTYPE "GET"


char* spawnReq(const char* url, const char* header){
	const char* path = strchr(url,'/');
	const char* format = "%s %s %s\r\n%s\r\n";
	char* request = (char*) calloc(strlen(REQTYPE)+strlen(path)+strlen(PROTOCOL)+strlen(header)+21, sizeof(char));
	sprintf(request,format,REQTYPE,path,PROTOCOL,header);
	return request;
}

// DO_NOT_EDIT (CAUTION : ENCODING)
char* spawnHeader(const char* hostname){
	char* header = (char*)calloc(strlen(hostname)+strlen(REFERER)+strlen(USER_AGENT)+strlen(COOKIE)+55, sizeof(char));
	//sprintf(header,"Host: %s\r\nReferer: %s\r\nUser-Agent: %s\r\n",hostname,REFERER,USER_AGENT);
	sprintf(header,"Host: %s\r\nReferer: %s\r\nUser-Agent: %s\r\nCookie: %s\r\n",hostname,REFERER,USER_AGENT,COOKIE);
	return header;
}

char* spawnHostName(const char* url){
	const char* path = strchr(url,'/');
	char* hostname = (char*) calloc(path-url+1, sizeof(char));
	strncpy(hostname, url, path-url);
	return hostname;
}
