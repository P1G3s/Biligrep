#include <ncurses.h>
#include <locale.h>
#include <stdlib.h>

#include "config.h"

#define B_NEXTPAGE 'l'
#define B_LASTPAGE 'h'
#define B_QUIT 'q'
#define B_UPKEY 'k'
#define B_DOWNKEY 'j'
#define B_CONFIRM 10
#define B_MAXLINES 25
#define B_MAXCOLUMNS 120


WINDOW* initUI();

char* showPlaylist(WINDOW* win,playlist* list);