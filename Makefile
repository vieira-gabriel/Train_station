CC = g++
CFLAGS = -g -Wall

all: train.o main.o
	$(CC) -o station main.o train.o -lm

train.o: Train.cpp Train.h
	$(CC) -c -o train.o Train.cpp $(CFLAGS)

main.o: main.cpp Train.h
	$(CC) -c -o main.o main.cpp $(CFLAGS)

clean:
	rm -f *.o
