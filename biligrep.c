#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>

#include "extractor.h"
#include "config.h"
#include "player.h"
#include "ui.h"
#include "utils.h"
#include <mpv/client.h>

char* historyOffset;
// This handler handles SIGWINCH which interrupt system call when resizing
void winchHandler(int num){}
void intHandler(int num){printf("\n\rExiting...\n");exit(0);}
void freePlaylist(playlist* list){
	BVideo* bvideos = list->bvideos;
	for(int i=0;i<list->size;i++){
		free(bvideos[i].title);
		free(bvideos[i].bid);
	}
	free(bvideos);
	free(list);
}

int main(int argc, char *argv[]){
	int index;
	int maxPage = 0;  // Max pages that have we got from the history dynamics
	int curPage = 0;  // Current page that we are in
	char* ret;

	signal(SIGWINCH, winchHandler);
	signal(SIGINT, intHandler);

	// TUI for dynamics
	WINDOW* win = initUI();
	playlist* list = extractLatestDynamic();
	do{
		ret = showPlaylist(win,list);
		switch(ret[0]){
			case B_NEXTPAGE:
				if (curPage==maxPage){
					freePlaylist(list);
					list = extractHistory(++curPage);
					maxPage++;
				}
				else{
					freePlaylist(list);
					list = getPlaylist(++curPage);
				}
				break;
			case B_LASTPAGE:
				if (curPage==0) break;
				else{
					freePlaylist(list);
					list = getPlaylist(--curPage);
				}
				break;
			case B_QUIT:
				printf("Exiting\n");
				endwin();
				return 0;
			default:
				index = ret[1];	
				break;
		}
		if ((ret[0]==B_CONFIRM)||(ret[0]==B_PLAYNOW)||(ret[0]==B_DOWNLOAD)) break;
	}while(1);
	wrefresh(win);
	endwin();

	//index = 1;
	BVideo* bvideos = list->bvideos;
	printf("Wonderful Choice! Greping %s! This might takes few sec...\n", bvideos[index].bid);

	if ((ret[0]==B_PLAYNOW)||ret[0]==B_CONFIRM){
		mpvPlay(bvideos[index]);	
	}

	if (ret[0]==B_DOWNLOAD){
		extractID(&bvideos[index]);
		char* playUrl = extractPlayUrl(&bvideos[index]);
		requestVideoStream(&bvideos[index],playUrl);

		free(bvideos[index].cid);
		free(bvideos[index].aid);
		free(playUrl);
	}

	freePlaylist(list);
	return 0;
}
