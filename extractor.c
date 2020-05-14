#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "config.h"
#include "spawner.h"
#include "net.h"
#include "utils.h"

void extractID(BVideo* bvideo){
	char** bid = &bvideo->bid;
	char** aid = &bvideo->aid;
	char** cid = &bvideo->cid;
	char** title = &bvideo->title;
	const char* format = "api.bilibili.com/x/web-interface/view?bvid=%s"; 
	char *url = (char*) calloc(strlen(format)+strlen(*bid)+1, sizeof(char));
	sprintf(url, format, bvideo->bid);
	char* hostname = spawnHostName(url);
	char* header = spawnHeader(hostname);
	char* request = spawnReq(url,header);
	int ClientSocketfd = httpConnect(hostname);
	sendReq(ClientSocketfd, request, ".buffer");

	free(header);free(request);free(url);free(hostname);

	bprint("[Extracting Info]");
	int fd = open(".buffer",O_RDONLY, S_IRUSR | S_IWUSR);
	const char* key[3];
	char* holder[3];
	key[0] = "aid";
	key[1] = "cid";
	key[2] = "title";
	grepJSONProp(fd, 3, key, holder);
	*aid = holder[0];
	*cid = holder[1];
	*title = holder[2];
	/* Extract */

	/*
		char *taid = strstr(response, "aid");	
		char *tcid = strstr(response, "cid");
		char *ttitle = strstr(response, "title");
		if(!(taid&&tcid)) Failed();
		if(!ttitle) ttitle="Unknown";
		strchr(taid, ',')[0] = '\0';
		strchr(tcid, ',')[0] = '\0';
		strchr(ttitle, ',')[0] = '\0';
		*aid = (char*) calloc(strlen(taid), sizeof(char));
		*cid = (char*) calloc(strlen(tcid), sizeof(char));
		*title = (char*) calloc(strlen(ttitle), sizeof(char));
		strcpy(*aid,taid+5);
		strcpy(*cid,tcid+5);
		strcpy(*title,ttitle+7);
	*/

	OJBK();
}


char* extractPlayUrl(BVideo* bvideo){
	const char* format = "api.bilibili.com/x/player/playurl?avid=%s&cid=%s&qn=80";
	char* url = (char*) calloc(strlen(format)+strlen(bvideo->aid)+strlen(bvideo->cid), sizeof(char));
	sprintf(url, format, bvideo->aid, bvideo->cid); 
	char* hostname = spawnHostName(url);
	char* header = spawnHeader(hostname);
	char* request = spawnReq(url,header);
	int ClientSocketfd = httpConnect(hostname);
	char* response = sendReq(ClientSocketfd, request, ".buffer");

	free(header);free(hostname);free(request);free(url);

	bprint("[Extrating PlayUrl]");
	if (response==NULL){
		int fd = open(".buffer",O_RDONLY, S_IRUSR | S_IWUSR);
		const char* key[1];
		char* holder[1];
		key[0] = "url";
		grepJSONProp(fd, 1, key, holder);
		response = holder[0];
	}
	/*
	if (response==NULL){
		char holder;
		int urlStart;
		int urlEnd;
		int fd = open(".buffer",O_RDONLY, S_IRUSR | S_IWUSR);
		ssize_t readlen = 0;
		// Extract first url only
		for(;;){
			if ((readlen = read(fd, &holder, 1))==-1) errExit("READ");
			if (holder == '/'){
				if ((urlStart = lseek(fd, 1, SEEK_CUR))==-1) errExit("lseek");
				do{
					if ((readlen = read(fd, &holder, 1))==-1) errExit("READ");
				}while(holder!='\"');
				urlEnd = lseek(fd, 0, SEEK_CUR);
				break;
			}
		}
		char* parsedUrl = (char*) calloc(urlEnd-urlStart+1, sizeof(char));
		char* start = parsedUrl;
		if (lseek(fd, urlStart, SEEK_SET)==-1) errExit("lseek");
		for(;;){
			if ((readlen = read(fd, &holder, 1))==-1) errExit("READ");
			if (urlEnd == lseek(fd, 0, SEEK_CUR)) break;
			if (holder == '\\'){
				if (lseek(fd, 5, SEEK_CUR)==-1) errExit("lseek");	
				*parsedUrl = '&';
			}
			else *parsedUrl = holder;
			parsedUrl++;
		}
		parsedUrl = start;
		close(fd);
		return parsedUrl;
	}	
	*/

	/* parsing */
	char* tPlayUrl = strstr(response,"//");
	if(!(tPlayUrl))	Failed();
	//strchr(tPlayUrl,'\"')[0] = '\0';	
	tPlayUrl += 2;
	char* start = tPlayUrl;
	char* parsedUrl = (char*)calloc(strlen(tPlayUrl), sizeof(char));
	char* location[12];
	for(int i=0;(tPlayUrl=strstr(tPlayUrl,"\\u0026"))!=NULL;i++){
		location[i] = tPlayUrl;
		tPlayUrl += 6;
	}
	tPlayUrl = start;
	int len = strlen(tPlayUrl);
	for(int i=0,j=0; i<len; i++){
		if(tPlayUrl == location[j]){
			tPlayUrl += 6;
			parsedUrl[i] = '&';
			j++;
		}
		else{
			parsedUrl[i] = *tPlayUrl++;
		}	
		if(*tPlayUrl == '\0') break;
	}
	OJBK();
	free(response);
	return parsedUrl;
}


playlist* extractLatestDynamic(){
	char* format = "api.vc.bilibili.com/dynamic_svr/v1/dynamic_svr/dynamic_new?uid=%s&type=268435455&from=header";
	char* url = (char*) calloc(strlen(format)+15, sizeof(char));
	//printf("%s\n",url);
	sprintf(url, format, UID);
	char* hostName = spawnHostName(url);
	char* header = spawnHeader(hostName);
	char* request = spawnReq(url, header);
	int ClientSocketfd = httpConnect(hostName);
	sendReq(ClientSocketfd, request, ".dynamic");
	free(hostName); free(header); free(request); free(url);

	// Extract Playlist
	// Refresh
	int dcachefd = open(".dcache", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	close(dcachefd);
	pushDynamics();
	playlist* list = getPlaylist(0);
	return list;
}

playlist* extractHistory(int page){
	char* format = "api.vc.bilibili.com/dynamic_svr/v1/dynamic_svr/dynamic_history?uid=%s&offset_dynamic_id=%s&type=268435455";
	char* url = (char*) calloc(strlen(format)+50, sizeof(char));
	extern char* historyOffset;
	sprintf(url, format, UID, historyOffset);
	//sprintf(url, format, UID); -> free() invalid next size!!!!
	char* hostName = spawnHostName(url);
	char* header = spawnHeader(hostName);
	char* request = spawnReq(url, header);
	int ClientSocketfd = httpConnect(hostName);
	sendReq(ClientSocketfd, request, ".dynamic");
	free(hostName); free(header); free(request); free(url);

	pushDynamics();
	playlist* list = getPlaylist(page);
	return list;
}


void requestVideoStream(BVideo* bvideo, const char* url){
	char* title = bvideo->title;
	char* hostname = spawnHostName(url);
	char* header = spawnHeader(hostname);
	char* request = spawnReq(url,header);
	sendDReq(httpConnect(hostname), request, title);
	free(title); free(hostname); free(header); free(request);
}

