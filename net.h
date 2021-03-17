#ifndef NET_H
#define NET_H

#include "config.h"

struct args_struct{
	ssize_t* readlen;
	ssize_t* contentLength;
	ssize_t* currentLength;
	ssize_t* writelenPerSec;
};

int httpConnect(const char* hostname);

void* progressMonitor(void* ptr);

char* sendReq(int ClientSocketfd, const char* request, const char* file);

void sendDReq(int ClientSocketfd, const char* request, const char* file);

#endif
