CC = g++
CFLAGS = -std=c++11

all: library_management

library_management: main.o
	$(CC) $(CFLAGS) main.o -o library_management

main.o: library_management.cpp
	$(CC) $(CFLAGS) -c library_management.cpp

clean:
	rm -f *.o library_management
