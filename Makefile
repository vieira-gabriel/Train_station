CC = g++
CFLAGS = -g -Wall


ifeq ($(mode),1)
	STATIONMODE := -DPRIORITY
endif
ifeq ($(mode),2)
	STATIONMODE := -DPRIORITY -DLOGON
endif

ifeq ($(limit),1)
	LIMITATION := -DLIMITED
endif



all: train.o main.o
	$(CC) $(STATIONMODE) $(LIMITATION) -o station main.o train.o -lm

train.o: Train.cpp Train.h
	$(CC) $(STATIONMODE) $(LIMITATION) -c -o train.o Train.cpp $(CFLAGS)

main.o: main.cpp Train.h
	$(CC) $(STATIONMODE) $(LIMITATION) -c -o main.o main.cpp $(CFLAGS)

clean:
	rm -f *.o
