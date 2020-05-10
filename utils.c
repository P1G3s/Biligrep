#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#include "config.h"

void grepJSONProp(int fd, int klen, const char** key, char** holder){
	char buf;
	char ban[klen];
	char done[klen];
	char alldone = 0;
	int START = lseek(fd, 0 ,SEEK_CUR);
	int END = lseek(fd, 0, SEEK_END);
	lseek(fd, START, SEEK_SET);
	memset(ban, 0, klen);
	memset(done, 0, klen);

	for (int i=0;i<klen;i++){
		holder[i] = NULL;
	}
	for (;;){
		read(fd, &buf, 1);
		// Property reached
	 	if (buf=='\"'){
			// Finding matched key
			read(fd, &buf, 1); 
			for(unsigned int i=0;;i++){
				for (int j=0; j<klen; j++){
					if ((done[j]==1)||(ban[j]==1)){
						continue;
					}
					if (i >= strlen(key[j])) {
						ban[j] = 1;
						continue;
					}
					if ((buf != key[j][i])){
						ban[j] = 1;
						continue;	
					}
				}
				//printf("%c",buf);
				read(fd, &buf, 1); 
				if (buf=='\"') break;
			}
			//printf("\n");
			
			// ForEech Key
			for (int i=0; i<klen; i++){
				// Key Matched
				if ((done[i]==0) && (ban[i]==0)){
					//printf("Matched Found : %s\n", key[i]);
					int startPos = 0;
					int endPos = 0;
					char escape = 0;
					char sym = 0;
					char counterSym;
					int cnt = 1;
					// Which type of symbol
					read(fd, &buf, 1);
					read(fd, &buf, 1);
					switch (buf){
						case '\"':
							sym = buf;
							counterSym = '\"';
							break;
						case '[':
							sym = buf;
							counterSym = ']';
							break;
						case '{':
							sym = buf;
							counterSym = '}';
							break;
						default: // Interger
							sym = 0;
							break;
					}
					startPos = lseek(fd, 0, SEEK_CUR)-1;
					while (read(fd, &buf, 1)>0){
						// Interger
						if (!sym) {
							if (buf != ',') continue;
							endPos = lseek(fd, 0, SEEK_CUR)-2;
							break;
						}
						if (sym != '\"'){
							// Skip rouge symbol
							if (buf == '\"'){
								while (read(fd, &buf, 1)){
								if (buf == '\\') {escape=1; continue;}
								if (escape) {escape=0; continue;}
								if (buf == '\"') break;
								}	
							}
							if (buf == sym)	cnt++;
							if (buf == counterSym) cnt--;
							if (cnt == 0) {endPos=lseek(fd, 0, SEEK_CUR)-1; break;}
						}
						else{
							// For "
							if (buf == '\\') {escape=1; continue;}
							if (escape) {escape=0; continue;}
							if (buf == counterSym) {endPos = lseek(fd, 0, SEEK_CUR)-1; break;}
						}
					}
					// No more comparison for this key
					done[i] = 1;
					// Remove 'edge'
					if (sym) {endPos-=1;startPos+=1;}
					int size = endPos-startPos;
					holder[i] = (char*) calloc(size+2, sizeof(char));	
					lseek(fd, startPos, SEEK_SET);
					read(fd, holder[i], size+1);
					// endPos+=1 trigger \" SKIP
					if (sym) {endPos+=2;startPos-=1;}
					lseek(fd, endPos, SEEK_SET);
					//printf("%s\n",holder[i]);
					break;
				}
			}
			// To next property
			int escape = 0;
			for (;;){
				read(fd, &buf, 1);
				// Under no match circumstances
				if (buf == ':'){
					read(fd, &buf, 1);
					if (buf == '\"'){
						while (read(fd, &buf, 1)){
							if (buf == '\\') {escape=1; continue;}
							if (escape) {escape=0; continue;}
							if (buf == '\"') break;
						}	
					}
					break;
				}
				// Under match circumstances
				if (buf == ',') break;
				if (END == lseek(fd,0,SEEK_CUR)) break;

			}
			memset(ban, 0, klen);
			// Check all done
			alldone = 1;
			for (int i=0; i<klen; i++){
				if (done[i]==0) {
					alldone=0;
					break;
				}
			}
		}
		if (alldone || (lseek(fd,0,SEEK_CUR)==END)) break;
		//lseek(fd, startPos, SEEK_SET);
	}
}


long grepContentLength(int ClientSocketfd){
	int buflen = 50;
	char holder;
	char buf[buflen];
	const char* str = "Content-Length:";
	char len = strlen(str);
	int isMatched = 0;
	int i = 0;
	for(;;){
		if(read(ClientSocketfd, &holder, 1)==-1) errExit("Failed to grep content length");
		/* reach line break */
		if(holder == '\n') {
			if(read(ClientSocketfd, &holder, 1)==-1) errExit("Failed to grep content length");
			// End of the response
			if(holder == '\r') {
				if(read(ClientSocketfd, &holder, 1)==-1) errExit("Failed to grep content length");
				break;
			}
		}
		/* Matche str */
		if(!isMatched && (holder == str[0])){
			isMatched = 1;
			for(int j=1; j<len; j++){
				if(read(ClientSocketfd, &holder, 1)==-1) errExit("Failed to grep content length");
				if(holder != str[j]){ isMatched=0; break; }
			}
			// Matched
			for(;isMatched;){
				if(read(ClientSocketfd, &holder, 1)==-1) errExit("Failed to grep content length");
				if(holder == '\r') { buf[i]='\0';break; }
				buf[i++] = holder;		
			}
		}
	}
	if(isMatched) return strtol(buf,NULL,0)+1;
	else{
		printf("\n[Content-Length: NOT FOUND]\n");
		return -1;
	}
}


// Get playlist from file(.dcache), the returned playlist should be freed by caller
playlist* getPlaylist(int page){
	playlist* list = malloc(sizeof(playlist));
	int dcachefd = open(".dcache", O_RDWR , S_IRUSR | S_IWUSR);
	char buf;
	int index = 0;
	BVideo* bvideos = calloc(1,sizeof(BVideo));
	for (int i=0;i<page;i++){
		while (read(dcachefd,&buf,1)){
			if (buf == '\r') break;
		}
	}
	off_t start = lseek(dcachefd, 0, SEEK_CUR);
	off_t end;
	while (read(dcachefd,&buf,1)>0){
		// Page end reached
		if (buf == '\r') break;
		if (buf == ' '){
			bvideos = realloc(bvideos,sizeof(BVideo)*(index+1));
			end = lseek(dcachefd, 0, SEEK_CUR);
			lseek(dcachefd, start, SEEK_SET);
			bvideos[index].bid = calloc(end-start, sizeof(char));
			for (int i=0; i<end-start-1; i++){
				read(dcachefd,&buf,1);
				(bvideos[index].bid)[i] = buf;
			}
			start = lseek(dcachefd, 1, SEEK_CUR);
			while (read(dcachefd,&buf,1)){
				if (buf == '\n'){
					end = lseek(dcachefd, 0, SEEK_CUR);
					lseek(dcachefd, start, SEEK_SET);
					bvideos[index].title = calloc(end-start+1, sizeof(char));
					for (int i=0; i<end-start-1; i++){
						read(dcachefd,&buf,1);
						(bvideos[index].title)[i] = buf;
					}
					break;
				}
			}
			start = lseek(dcachefd, 1, SEEK_CUR);
			//bvideo = realloc(bvideo, sizeof(bvideo));???????
			index++;
		}
	}
	list->bvideos = bvideos;
	list->size = index;
	return list;
}


void pushDynamics(){
	char* holder[1];
	const char* key[1];
	// Extract "cards"
	int dynamicfd = open(".dynamic",O_RDONLY, S_IRUSR | S_IWUSR);
	int cardsfd = open(".cards", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	int descfd = open(".desc", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	int dcachefd = open(".dcache", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	int curPos;
	int holderlen;

	lseek(dynamicfd, 0, SEEK_SET);
	*key = "cards";
	grepJSONProp(dynamicfd, 1, key, holder);
	write(cardsfd, holder[0], strlen(holder[0]));
	lseek(cardsfd, 0, SEEK_SET);
	free(*holder);

	if (!historyOffset){
		*key = "history_offset";
		grepJSONProp(dynamicfd, 1, key, holder);
		historyOffset = *holder;
	}else{
		free(historyOffset);
		*key = "next_offset";
		grepJSONProp(dynamicfd, 1, key, holder);
		historyOffset = *holder;
	}
	// Collect "desc" and "card
	lseek(dcachefd, 0, SEEK_END);
	for (int i=0;;i++){
		*key = "desc";
		curPos = lseek(descfd, 0, SEEK_CUR);
		grepJSONProp(cardsfd, 1, key, holder);
		if (*holder == NULL) {break;}
		write(descfd, *holder, strlen(*holder));
		write(descfd,"\n", 1);
		free(*holder);

		*key = "card";
		grepJSONProp(cardsfd, 1, key, holder);
		if (*holder == NULL) {printf("%s not found",key[0]);break;}
		holderlen = strlen(*holder);
		// Remove quote
		for (int i=0; i<holderlen; i++){
			if ((*holder)[i] == '\\') continue;
			write(descfd, &((*holder)[i]), 1);
		}
		write(descfd,"\n\n", 2);
		free(*holder);
		
		*key = "type";
		lseek(descfd, curPos, SEEK_SET);
		grepJSONProp(descfd, 1, key, holder);
		if (*holder == NULL) {printf("%s not found",key[0]);break;}

		if (!strcmp(*holder,"8")){
			//printf("%d * %ld\n",(list->size),sizeof(BVideo));
			//bvideos = realloc(bvideos, (list->size)*sizeof(BVideo));
			free(*holder);
			*key = "bvid";
			lseek(descfd, curPos, SEEK_SET);
			grepJSONProp(descfd, 1, key, holder);
			if (*holder == NULL) {printf("%s not found",key[0]);break;}
			write(dcachefd, *holder, strlen(*holder));
			write(dcachefd, " ", 1);

			*key = "title";
			lseek(descfd, curPos, SEEK_SET);
			grepJSONProp(descfd, 1, key, holder);
			if (*holder == NULL) {printf("%s not found",key[0]);break;}
			write(dcachefd, *holder, strlen(*holder));
			write(dcachefd, "\n", 1);
		}
		else{
			free(*holder);
		}
		lseek(descfd, 0, SEEK_END);
	}
	// Each page will end with a '\r'
	write(dcachefd, "\r", 1);
	close(descfd);
	close(cardsfd);
	close(dcachefd);
	close(dynamicfd);
}
