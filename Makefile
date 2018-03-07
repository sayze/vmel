CC=gcc
OUTD=bin
SRC=src
CFLAGS=-Isrc/

all:
	$(CC) src/vmel.c -o $(OUTD)/vmel $(CFLAGS)

clean:
	rm -rf bin/*