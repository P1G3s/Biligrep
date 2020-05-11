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

int httpConnect(const char* hostname){
	struct sockaddr_in saddr;
	struct hostent *host;
	int ClientSocketfd;

	//bprint("[Connecting To Host]");
	/* create socket for client */
	ClientSocketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (ClientSocketfd==-1) errExit("Failed to create socket");
	/* connect */
	memset(&saddr,0,sizeof(saddr));
	host = (struct hostent *) gethostbyname(hostname);
	if(host == NULL) Failed();
	saddr.sin_port = htons(80);
	saddr.sin_family = AF_INET;
	memcpy(&saddr.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
	if (connect(ClientSocketfd, (struct sockaddr *)&saddr, sizeof(saddr)) == -1) errExit("Failed to connect");
	//OJBK();
	return ClientSocketfd;
}


void *progressMonitor(void* ptr){
	struct args_struct* args = (struct args_struct*) ptr;
	ssize_t* writelenPerSec = args->writelenPerSec;
	ssize_t* contentLength = args->contentLength;
	ssize_t* currentLength = args->currentLength;
	int backlen = 55;
	int barlen = 21;
	char* bar = (char*)calloc(barlen, sizeof(char));
	char* back = (char*)calloc(backlen+1, sizeof(char));
	memset(back, '\b', backlen);
	memset(bar, ' ', barlen-1);
	printf("\n");
	for(;;){
		sleep(1);
		int percentage = ((*currentLength)*10)/(*contentLength);
		memset(bar, '#', percentage*2);
		printf("[%s]      ",bar);
		printf("Speed:%6ld KB/s", (*writelenPerSec/1024));
		fflush(NULL);
		*writelenPerSec = 0;
		if ((*currentLength) == (*contentLength)) break;		
		printf("\r");
	}
	free(bar);
	free(back);
	printf("\n");
	bprint("[Downloading]");
	OJBK();

	return NULL;
}


// Send normal request
char* sendReq(int ClientSocketfd, const char* request, const char* file){
	long contentLength;
	long responseLength;
	ssize_t readlen = 0;
	ssize_t currentLength = 1;
	char* response;
	/* transfer request */
	if (write(ClientSocketfd, request, strlen(request))==-1) errExit("Failed to shoot the request");
	/* read/output */
	contentLength = grepContentLength(ClientSocketfd);
	//printf("[Content-Length: %ld]\n",contentLength);
	//printf("\n----------Content Of The Response----------\n");
	if (file==NULL){
		// ContentLength unknown (no response will be returned, contents go to .buffer file)
		if (contentLength==-1) {
			char* buffer = (char*) calloc(MAXLEN, sizeof(char));
			int fd = open(".buffer", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
			while((readlen=read(ClientSocketfd, buffer, MAXLEN))>0){
				if(write(fd, buffer, readlen)==-1) errExit("WRITE");
				if(write(STDOUT_FILENO, buffer, readlen)==-1) errExit("WRITE");
			}
			response =  NULL;
			free(buffer);
		}
		// ContentLength known (response returned)
		else{
			responseLength = contentLength<MAXLEN?contentLength:MAXLEN;
			response = (char*) calloc(responseLength+1, sizeof(char));
			char* start = response;
			while((readlen=read(ClientSocketfd, response, responseLength))>0){
				if(write(STDOUT_FILENO, response, readlen)==-1) errExit("WRITE");
				response += readlen;
				currentLength += readlen;
				if(currentLength >= contentLength) break;
			}
			response = start;
		}
	}
	// Write to file
	else{
		int bufferLength = ((contentLength<MAXLEN)&&(contentLength!=-1))?contentLength:MAXLEN;
		int writelen = 0;
		char* buffer = (char*) calloc(bufferLength, sizeof(char));
		int fd = open(file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
		while(((readlen=read(ClientSocketfd, buffer, bufferLength))>0)){
			if (write(fd, buffer, readlen)==-1) errExit("FILE WRITE");
			writelen += readlen;
			if (writelen+1 == contentLength) break;
			//if(write(STDOUT_FILENO, buffer, readlen)==-1) errExit("WRITE");
		}
		free(buffer);
		response = NULL;
	}
	//printf("\n-------------------------------------------\n");
	bprint("[Closing  Connection]");
	if(close(ClientSocketfd)==-1) errExit("Close Connection");
	OJBK();
	printf("\n");
	return response;
}

// Send request for downloading video stream
void sendDReq(int ClientSocketfd, const char* request, const char* file){
	long contentLength;
	long responseLength;
	ssize_t readlen = 0;
	ssize_t currentLength = 1;
	ssize_t writelenPerSec = 0;
	char* response;
	/* transfer request */
	if (write(ClientSocketfd, request, strlen(request))==-1) errExit("Failed to shoot the request");
	/* read/output */
	contentLength = grepContentLength(ClientSocketfd);
	printf("[Content-Length: %ld]\n",contentLength);
	responseLength = contentLength<MAXLEN?contentLength:MAXLEN;
	response = (char*) calloc(responseLength+1, sizeof(char));
	int fd = open("Video1.flv", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	struct args_struct args;
	args.readlen = &readlen;
	args.currentLength = &currentLength;
	args.contentLength = &contentLength;
	args.writelenPerSec = &writelenPerSec;
	pthread_t pgbar;
	pthread_create( &pgbar, NULL, &progressMonitor, &args);
	printf("[Total File Size: %ld MB]\n",(contentLength/1024)/1024);
	printf("[Downloading - \"%s\"]\n", file);
	do{
		readlen = read(ClientSocketfd, response, responseLength);
		currentLength += readlen;
		writelenPerSec += readlen;
		if (write(fd, response, readlen)==-1) errExit("Error occurs during downloading, Try agin");
	}while((currentLength<contentLength)&&(readlen!=0));
	pthread_join(pgbar, NULL);
	close(fd);
	bprint("[Closing  Connection]");
	if(close(ClientSocketfd)==-1) errExit("Close Connection");
	OJBK();
	free(response);
	printf("\n");
}
