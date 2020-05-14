#include <ncurses.h>
#include <locale.h>
#include <stdlib.h>

#include "config.h"

#define B_DOWNLOAD 'd'
#define B_PLAYNOW 'p'
#define B_NEXTPAGE 'l'
#define B_LASTPAGE 'h'
#define B_QUIT 'q'
#define B_UPKEY 'k'
#define B_DOWNKEY 'j'
#define B_CONFIRM 10
#define B_MAXLINES 25
#define B_MAXCOLUMNS 100

WINDOW* initUI(){
	setlocale(LC_ALL,"");
	initscr();
	noecho();
	cbreak();
	clear();
	refresh();
	WINDOW* win = newwin(B_MAXLINES, B_MAXCOLUMNS, 1, 1);
	keypad(win,TRUE);
	return win;
}

// Show playlist, and return [type, off_y]
char* showPlaylist(WINDOW* win,playlist* list){
	// Coordinates of the first item (not border)
	int start_x=1;
	int start_y=1;
	//int off_x = 0;
	int off_y = 0;
	int done = 0;
	char input;
	char* ret = malloc(sizeof(char)*2);
	ret[0]=0;ret[1]=0;
	BVideo* bvideos = list->bvideos;
	//WINDOW* win = newwin(B_MAXLINES, B_MAXCOLUMNS, 1, 1);
	wclear(win);
	wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
	for(int i=0;i<list->size;i++){
		mvwprintw(win, start_y+i, start_x, "%2d. %s", i+1, bvideos[i].title);
	}
	wmove(win, start_y, start_x);
	wrefresh(win);
	//wrefresh(win);	
	for(;!done;){
		input = (char)wgetch(win);
		switch(input){
			case B_DOWNKEY:
				off_y = off_y==list->size-1?off_y:off_y+1; 
				wmove(win, start_y+off_y, start_x);
				wrefresh(win);
				break;
			case B_UPKEY:
				off_y = off_y==0?off_y:off_y-1;
				wmove(win, start_y+off_y, start_x);
				wrefresh(win);
				break;
			case B_DOWNLOAD:
				ret[0] = B_DOWNLOAD;
				break;
			case B_PLAYNOW:
				ret[0] = B_PLAYNOW;
				break;
			case B_CONFIRM:
				ret[0] = B_CONFIRM;
				break;
			case B_NEXTPAGE:
				ret[0] = B_NEXTPAGE;
				break;
			case B_LASTPAGE:
				ret[0] = B_LASTPAGE;
				break;
			case B_QUIT:
				ret[0] = B_QUIT;
				break;
			default: 
				break;
		}
		if(ret[0]!=0) {ret[1]=off_y;break;}
	}
	wclear(win);
	wrefresh(win);
	return ret;
}
