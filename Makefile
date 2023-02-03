CC=gcc

CFLAGS=-Wall -Wextra -pedantic -std=c11

all: gameoflife libgol.so

gameoflife: gameoflife.o libgol.so
	$(CC) $(CFLAGS) -L. -lgol gameoflife.o -o gameoflife

libgol.so: gol.o
	$(CC) $(CFLAGS) -shared gol.o -o libgol.so

gameoflife.o: gameoflife.c
	$(CC) -c $(CFLAGS) gameoflife.c

gol.o: gol.c gol.h
	$(CC) -fPIC $(CFLAGS) -c gol.c

clean:
	rm *.o gameoflife libgol.so
