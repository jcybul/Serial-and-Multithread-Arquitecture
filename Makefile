CC = gcc
CFLAGS = -g -std=gnu99
THRD = -lpthread


all:
	$(CC) $(CFLAGS) server.c -o server -lpthread

clean:
	 rm server *.o
