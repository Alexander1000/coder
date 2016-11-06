# compiler
CC=g++

# flags
CFLAGS=-c -Wall

all: coder

coder: main.o
	$(CC) main.o -o coder

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

clean:
	rm -rf *.o coder