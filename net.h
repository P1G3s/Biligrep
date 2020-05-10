#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>

#include "config.h"
#include "utils.h"

#define MAXLEN 8192

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
