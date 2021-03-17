#ifndef CONFIG_H
#define CONFIG_H

#define UID "14066157"
#define PORT 80

#define errExit(s) {perror(s);exit(1);}
#define OJBK() printf("\x1B[32mOK\x1B[0m\n");
#define bprint(s) {printf("%-60s",s);fflush(NULL);}
#define Failed() {printf("\x1B[31mFailed\n\x1B[0m");exit(1);}
struct BVideo{
	char* bid;
	char* aid;
	char* cid;
	char* title;
	char* dynamicid;
};
typedef struct BVideo BVideo;

struct playlist{
	BVideo* bvideos;
	int size;
};
typedef struct playlist playlist;

#endif
