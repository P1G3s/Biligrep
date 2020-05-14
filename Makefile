CFLAGS = -pthread -fno-common
LIB = -lncursesw `pkg-config --libs --cflags mpv`

CC = gcc

OBJ = spawner.o net.o extractor.o utils.o player.o biligrep.o ui.o

all: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LIB) -o biligrep 

spawner.o : spawner.h spawner.c header.h
	$(CC) -c spawner.c
net.o : net.h net.c
	$(CC) -c net.c
extractor.o : extractor.h extractor.c
	$(CC) -c extractor.c
utils.o : utils.h utils.c
	$(CC) -c utils.c
player.o : player.h player.c 
	$(CC) -c player.c 
ui.0: ui.h ui.c
	$(CC) -c ui.c
biligrep.o: biligrep.c biligrep.c
	$(CC) -c biligrep.c


.PHONY = clean

clean:
	rm *.o biligrep
