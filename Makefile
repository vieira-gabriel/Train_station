CC = g++
CFLAGS = -g -Wall


ifeq ($(mode),1)
	STATIONMODE := -DPRIORITY
endif

all: train.o main.o
	$(CC) $(STATIONMODE) -o station main.o train.o -lm

train.o: Train.cpp Train.h
	$(CC) $(STATIONMODE) -c -o train.o Train.cpp $(CFLAGS)

main.o: main.cpp Train.h
	$(CC) $(STATIONMODE) -c -o main.o main.cpp $(CFLAGS)

clean:
	rm -f *.o
