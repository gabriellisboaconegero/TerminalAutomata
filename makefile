CC = gcc
CFLAGS = -g -Wall -Wextra

all: main.c termal.o
	$(CC) $(CFLAGS) $^ -o main

termal.o: termal.c
	$(CC) $(CFLAGS) -c $^ -o $@

clear:
	rm -rf *.o
