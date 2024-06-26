CC=gcc
CFLAGS=-Wall -pthread

all: server client

server: server.o
	$(CC) $(CFLAGS) server.o -o server

server.o: server.c
	$(CC) $(CFLAGS) -c server.c

client: client.o
	$(CC) $(CFLAGS) client.o -o client

client.o: client.c
	$(CC) $(CFLAGS) -c client.c

clean:
	rm -f *.o server client

.PHONY: all clean
