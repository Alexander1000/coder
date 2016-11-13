# compiler
CC=g++

# flags
CFLAGS=-c -Wall

all: coder clean

coder: main.o
	$(CC) main.o -o coder

main.o:
	$(CC) $(CFLAGS) main.cpp

clean:
	rm -rf *.o