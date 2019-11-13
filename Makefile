CC=gcc
CFLAGS=-g -Wall
OUT=nb0-extract

all: main.c nb0.o
	$(CC) $(CFLAGS) main.c nb0.o -o $(OUT)

clean:
	rm *.o
	rm $(OUT)

nb0.o : nb0.c nb0.h
	$(CC) $(CFLAGS) -c nb0.c

util.o : util.c util.h
	$(CC) $(CFLAGS) -c util.c

