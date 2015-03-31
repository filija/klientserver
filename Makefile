#MAKEFILE client/server

CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -pedantic

all: server client

server: server.c
	$(CC) $(CFLAGS) -o server server.c

client: client.c
	$(CC) $(CFLAGS) -o client client.c
