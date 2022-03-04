# Makefile for 'grid' module
#
# Emily Gao

OBJS = grid.o gridTester.o
LIBS = ../libcs50/libcs50-given.a 

# uncomment the following to turn on verbose memory logging
#TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I ../libcs50 
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

gridTester: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

gridTester.o: ../libcs50/libcs50-given.a grid.h
grid.o: ../libcs50/libcs50-given.a grid.h

.PHONY: test valgrind clean

# expects a file `hashtableFile.txt` to exist; it can contain any text.
test: gridTester 
	./gridTester ../maps/small.txt

# expects a file `hashtableFile.txt` to exist; it can contain any text.
valgrind: gridTester ../maps/small.txt
	$(VALGRIND) ./gridTester

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f grid
	rm -f gridTester
	rm -f core
